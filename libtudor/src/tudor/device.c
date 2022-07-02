#include <unistd.h>
#include "internal.h"

static NTSTATUS tudor_devctrl(struct tudor_device *device, ULONG code, void *in_buf, size_t in_size, void *out_buf, size_t *out_size) {
    struct winmodule *mod = winmodule_get_cur();
    winmodule_set_cur(&tudor_driver_dll->module);
    NTSTATUS status = winwdf_devctrl_file(device->wdf_file, code, in_buf, in_size, out_buf, out_size);
    winmodule_set_cur(mod);
    log_verbose("DEVCTRL 0x%x in 0x%lx out 0x%lx status 0x%x", code, in_size, *out_size, status);
    return status;
}

bool tudor_open(struct tudor_device *device, libusb_device_handle *usb_dev, struct tudor_device_state *state, tudor_reenumerate_fnc *reenum_fnc, void *reenum_ctx) {
    HRESULT hres;
    NTSTATUS status;

    device->state = state ? *state : (struct tudor_device_state) {0};
    device->enrolling = false;
    cant_fail(pthread_mutex_init(&device->records_lock, NULL));
    device->records_head = NULL;
    device->result_records_head = device->result_records_cursor = NULL;

    //Reset the USB device
    int usb_err;
    if((usb_err = libusb_reset_device(usb_dev)) != 0) {
        if(usb_err == LIBUSB_ERROR_NOT_FOUND) {
            //Re-enumerate the device
            log_info("Re-enumerating USB device...");
            usb_dev = reenum_fnc(reenum_ctx);
            if(!usb_dev) return false;
        } else {
            log_error("libusb_reset_device failed: %d [%s]", usb_err, libusb_error_name(usb_err));
            return false;
        }
    }

    //Open the device through the driver
    device->reg_key = winreg_open_key(device, "HKEY_LOCAL_MACHINE\\Tudor\\Device");
    if((status = winwdf_add_device(tudor_wdf_driver, device->reg_key, usb_dev, &device->wdf_device)) != 0) {
        log_error("Error adding WDF device: 0x%x!", status);
        return false;
    }
    if(!device->wdf_device) {
        log_error("Driver didn't create a WDF device!");
        return false;
    }
    winwdf_event_queue_flush();

    if((status = winwdf_open_device(device->wdf_device, &device->wdf_file)) != 0) {
        log_error("Error opening WDF file: 0x%x!", status);
        return false;
    }

    //This is dumb, but otherwise we run into race conditions
    cant_fail(usleep(3000000));

    //Initialize the pipeline
    winmodule_set_cur(&tudor_adapter_dll->module);

    log_debug("Initializing WINBIO pipeline...");
    device->pipeline = (WINBIO_PIPELINE*) malloc(sizeof(WINBIO_PIPELINE));
    if(!device->pipeline) { perror("Error allocating WINBIO pipeline"); abort(); }
    device->pipeline->EngineInterface = tudor_engine_adapter;
    device->pipeline->SensorInterface = tudor_sensor_adapter;
    device->pipeline->StorageInterface = tudor_storage_adapter;
    device->pipeline->SensorHandle = device->winbio_file = winio_create_file(device, true, NULL, NULL, (winio_devctrl_fnc*) tudor_devctrl, NULL);
    device->pipeline->EngineHandle = INVALID_HANDLE_VALUE;
    device->pipeline->StorageHandle = INVALID_HANDLE_VALUE;
    device->pipeline->StorageContext = device;

    log_debug("Attaching interfaces to pipeline...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->Attach, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->Attach, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->Attach, device->pipeline);

    log_debug("Initializing pipeline interfaces...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->PipelineInit, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->PipelineInit, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->PipelineInit, device->pipeline);

    //Reset the sensor
    log_debug("Resetting sensor...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->Reset, device->pipeline);

    //Activate the pipeline
    log_debug("Activating pipeline...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->Activate, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->Activate, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->Activate, device->pipeline);

    //Check the sensor status
    log_debug("Checking sensor status...");
    ULONG sensor_status = WINBIO_SENSOR_FAILURE;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->QueryStatus, device->pipeline, &sensor_status)
    if(sensor_status != WINBIO_SENSOR_READY) {
        log_error("Sensor didn't return ready status! [status 0x%x]", status);
        return false;
    }

    return true;
}

bool tudor_close(struct tudor_device *device) {
    HRESULT hres;

    winmodule_set_cur(&tudor_adapter_dll->module);

    //Deactivate the pipeline
    log_debug("Deactivating pipeline...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->Deactivate, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->Deactivate, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->Deactivate, device->pipeline);

    //Uninitialize the pipeline
    log_debug("Uninitializing pipeline interfaces...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->PipelineCleanup, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->PipelineCleanup, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->PipelineCleanup, device->pipeline);

    log_debug("Detaching interfaces from pipeline...");
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->Detach, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->Detach, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->Detach, device->pipeline);
    free(device->pipeline);

    winhandle_destroy(device->winbio_file);

    //Close the WDF file
    winmodule_set_cur(&tudor_driver_dll->module);
    winwdf_close_file(device->wdf_file);

    //Remove the device
    winmodule_set_cur(&tudor_driver_dll->module);
    winwdf_remove_device(device->wdf_device);
    winwdf_event_queue_flush();
    winhandle_destroy(device->reg_key);

    //Free records
    cant_fail(pthread_mutex_lock(&device->records_lock));
    for(struct tudor_record *rec = device->records_head, *nrec = rec ? rec->next : NULL; rec; rec = nrec, nrec = rec ? rec->next : NULL) {
        free(rec->data);
        free(rec->identity);
        free(rec);
    }
    cant_fail(pthread_mutex_unlock(&device->records_lock));
    cant_fail(pthread_mutex_destroy(&device->records_lock));

    return true;
}

bool tudor_enroll_start(struct tudor_device *device, RECGUID guid, enum tudor_finger finger) {
    HRESULT hres;

    if(device->enrolling) {
        log_error("Already enrolling a finger!");
        return false;
    }

    //Follow https://docs.microsoft.com/en-us/windows/win32/secbiomet/adapter-workflow - WinBioEnrollBegin
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->ClearContext, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->ClearContext, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_storage_adapter->ClearContext, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->CreateEnrollment, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->SetEnrollmentParameters, device->pipeline, &(WINBIO_EXTENDED_ENROLLMENT_PARAMETERS) {
        .Size = sizeof(WINBIO_EXTENDED_ENROLLMENT_PARAMETERS),
        .SubFactor = (UCHAR) finger
    });

    device->enrolling = true;
    device->enroll_guid = guid;
    device->enroll_finger = finger;
    return true;
}

bool tudor_enroll_capture(struct tudor_device *device, bool *done) {
    HRESULT hres;
    NTSTATUS status;

    *done = true;
    if(!device->enrolling) {
        log_error("Not currently enrolling a finger!");
        return false;
    }

    //Follow https://docs.microsoft.com/en-us/windows/win32/secbiomet/adapter-workflow - WinBioEnrollCapture
    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_ENROLL, &ovlp);
    if((status = winio_wait_overlapped(ovlp, NULL)) != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        return false;
    }

    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(device->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        if(hres == WINBIO_E_BAD_CAPTURE) *done = false;
        return false;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(device->pipeline, WINBIO_PURPOSE_ENROLL, 0, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        if(hres == WINBIO_E_BAD_CAPTURE) *done = false;
        return false;
    };

    log_debug("Updating enrollment...");
    if((hres = tudor_engine_adapter->UpdateEnrollment(device->pipeline, &reject_detail)) != ERROR_SUCCESS && hres != WINBIO_I_MORE_DATA) {
        log_error("Error updating enrollment: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        if(hres == WINBIO_E_BAD_CAPTURE) *done = false;
        return false;
    };

    *done = hres != WINBIO_I_MORE_DATA;
    return true;
}

bool tudor_enroll_commit(struct tudor_device *device, bool *is_duplicate) {
    HRESULT hres;

    *is_duplicate = false;
    if(!device->enrolling) {
        log_error("Not currently enrolling a finger!");
        return false;
    }

    //Follow https://docs.microsoft.com/en-us/windows/win32/secbiomet/adapter-workflow - WinBioEnrollCommit
    log_debug("Checking for duplicate enrollment...");
    BOOLEAN is_dupl;
    WINBIO_IDENTITY dupl_ident;
    UCHAR dupl_finger;
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->CheckForDuplicate, device->pipeline, &dupl_ident, &dupl_finger, &is_dupl);
    if(is_dupl) {
        *is_duplicate = true;
        return false;
    }

    log_debug("Committing enrollment...");
    //The driver doesn't support enrollment hashes (so we don't call GetEnrollmentHash)
    if((hres = tudor_engine_adapter->CommitEnrollment(device->pipeline, &(WINBIO_IDENTITY) {
        .Type = WINBIO_ID_TYPE_GUID,
        .TemplateGuid = *(GUID*) &device->enroll_guid
    }, (UCHAR) device->enroll_finger, NULL, 0)) != ERROR_SUCCESS) {
        log_error("Error commiting enrollment: 0x%x!", hres);
        if(hres == WINBIO_E_DUPLICATE_ENROLLMENT) *is_duplicate = true;
        return false;
    }

    device->enrolling = false;
    return true;
}

bool tudor_enroll_discard(struct tudor_device *device)  {
    HRESULT hres;

    if(!device->enrolling) {
        log_error("Not currently enrolling a finger!");
        return false;
    }

    log_info("Discarding enrollment of guid=%08x... finger=%x", device->enroll_guid.PartA, device->enroll_finger);

    //Follow https://docs.microsoft.com/en-us/windows/win32/secbiomet/adapter-workflow - WinBioEnrollDiscard
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->DiscardEnrollment, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_engine_adapter->ClearContext, device->pipeline);
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->ClearContext, device->pipeline);

    device->enrolling = false;
    return true;
}

bool tudor_verify(struct tudor_device *device, RECGUID guid, enum tudor_finger finger, bool *matches) {
    HRESULT hres;
    NTSTATUS status;

    *matches = false;
    if(device->enrolling) {
        log_error("Currently enrolling a finger!");
        return false;
    }

    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_VERIFY, &ovlp);
    if((status = winio_wait_overlapped(ovlp, NULL)) != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        return false;
    }

    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(device->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(device->pipeline, WINBIO_PURPOSE_VERIFY, 0, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };

    log_debug("Verifying sample...");
    BOOLEAN is_match;
    UCHAR *payload_ptr, *hash_ptr;
    SIZE_T payload_size, hash_size;
    if((hres = tudor_engine_adapter->VerifyFeatureSet(device->pipeline, &(WINBIO_IDENTITY) {
        .Type = WINBIO_ID_TYPE_GUID,
        .TemplateGuid = *(GUID*) &guid
    }, (UCHAR) finger, &is_match, &payload_ptr, &payload_size, &hash_ptr, &hash_size, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_NO_MATCH) {
            *matches = false;
            return true;
        }

        log_error("Error verifying sample: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };
    *matches = is_match;

    return true;
}

bool tudor_identify(struct tudor_device *device, bool *found_match, RECGUID *guid, enum tudor_finger *finger) {
    HRESULT hres;
    NTSTATUS status;

    *found_match = false;
    if(device->enrolling) {
        log_error("Currently enrolling a finger!");
        return false;
    }

    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_IDENTIFY, &ovlp);
    if((status = winio_wait_overlapped(ovlp, NULL)) != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        return false;
    }

    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(device->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(device->pipeline, WINBIO_PURPOSE_IDENTIFY, 0, &reject_detail)) != ERROR_SUCCESS) {
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };

    log_debug("Identifying sample...");
    WINBIO_IDENTITY identity;
    UCHAR subfactor;
    UCHAR *payload_ptr, *hash_ptr;
    SIZE_T payload_size, hash_size;
    if((hres = tudor_engine_adapter->IdentifyFeatureSet(device->pipeline, &identity, &subfactor, &payload_ptr, &payload_size, &hash_ptr, &hash_size, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_UNKNOWN_ID) {
            *found_match = false;
            return true;
        }

        log_error("Error identifying sample: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        return false;
    };
    if(identity.Type != WINBIO_ID_TYPE_GUID) {
        log_error("Invalid identification ID type: %d!", identity.Type);
        return false;
    }

    *found_match = true;
    *guid = *(RECGUID*) &identity.TemplateGuid;
    *finger = (enum tudor_finger) subfactor;

    return true;
}