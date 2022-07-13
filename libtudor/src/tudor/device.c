#include <unistd.h>
#include "internal.h"

static void req_cb(struct winwdf_request *req, NTSTATUS status, OVERLAPPED *ovlp) {
    //Get request info
    const void *out_buf = NULL;
    size_t out_buf_size, num_transfered = 0;
    if(status == STATUS_SUCCESS) {
        if(!winwdf_get_request_info(req, NULL, NULL, NULL, &out_buf, &out_buf_size, &num_transfered)) {
            log_error("Couldn't get request info!");
            abort();
        }
        if(num_transfered > out_buf_size) num_transfered = out_buf_size;
    }

    if(LOG_LEVEL <= LOG_VERBOSE) {
        cant_fail_ret(pthread_mutex_lock(&LOG_LOCK));
        printf("[DEVCTRL] <- status 0x%x out (size 0x%lx): ", status, num_transfered);
        if(status == STATUS_SUCCESS) for(size_t i = 0; i < num_transfered; i++) printf("%02x", ((uint8_t*) out_buf)[i]);
        puts("");
        cant_fail_ret(pthread_mutex_unlock(&LOG_LOCK));
    }

    //Complete the OVERLAPPED
    winio_complete_overlapped(ovlp, status, num_transfered);
}

static NTSTATUS tudor_devctrl(struct tudor_device *device, OVERLAPPED *ovlp, ULONG code, void *in_buf, size_t in_size, void *out_buf, size_t out_size, struct winwdf_request **req) {
    if(LOG_LEVEL <= LOG_VERBOSE) {
        cant_fail_ret(pthread_mutex_lock(&LOG_LOCK));
        printf("[DEVCTRL] -> in code 0x%x (size 0x%lx): ", code, in_size);
        for(size_t i = 0; i < in_size; i++) printf("%02x", ((uint8_t*) in_buf)[i]);
        puts("");
        cant_fail_ret(pthread_mutex_unlock(&LOG_LOCK));
    }

    //Start the request
    struct winmodule *mod = winmodule_get_cur();
    winmodule_set_cur(&tudor_driver_dll->module);
    NTSTATUS status = winwdf_devctrl_file(device->wdf_file, code, in_buf, in_size, out_buf, out_size, req);
    winmodule_set_cur(mod);

    //Add callback
    if(status == STATUS_SUCCESS) winwdf_add_request_callback(*req, (winwdf_request_cb_fnc*) req_cb, ovlp);

    return status;
}

static NTSTATUS tudor_cancel(struct tudor_device *device, OVERLAPPED *ovlp, struct winwdf_request *req) {
    winwdf_cancel_request(req);
    return STATUS_SUCCESS;
}

static void tudor_cleanup(struct tudor_device *device, OVERLAPPED *ovlp, struct winwdf_request *req) {
    winwdf_destroy_object((WDFOBJECT) req);
}

bool tudor_open(struct tudor_device *device, libusb_device_handle *usb_dev, struct tudor_device_state *state) {
    HRESULT hres;
    NTSTATUS status;

    device->state = state ? *state : (struct tudor_device_state) {0};
    device->enrolling = false;
    cant_fail_ret(pthread_mutex_init(&device->records_lock, NULL));
    device->records_head = NULL;
    device->result_records_head = device->result_records_cursor = NULL;

    //Reset the USB device
    int usb_err;
    if((usb_err = libusb_reset_device(usb_dev)) != 0) {
        log_error("libusb_reset_device failed: %d [%s]", usb_err, libusb_error_name(usb_err));
        return false;
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
    *device->pipeline = (WINBIO_PIPELINE) {0};
    device->pipeline->EngineInterface = tudor_engine_adapter;
    device->pipeline->SensorInterface = tudor_sensor_adapter;
    device->pipeline->StorageInterface = tudor_storage_adapter;
    device->pipeline->SensorHandle = device->winbio_file = winio_create_file(device, true, NULL, NULL, (winio_devctrl_fnc*) tudor_devctrl, (winio_cancel_fnc*) tudor_cancel, (winio_cleanup_fnc*) tudor_cleanup, NULL);
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
    cant_fail_ret(pthread_mutex_lock(&device->records_lock));
    for(struct tudor_record *rec = device->records_head, *nrec = rec ? rec->next : NULL; rec; rec = nrec, nrec = rec ? rec->next : NULL) {
        free(rec->data);
        free(rec->identity);
        free(rec);
    }
    cant_fail_ret(pthread_mutex_unlock(&device->records_lock));
    cant_fail_ret(pthread_mutex_destroy(&device->records_lock));

    return true;
}

bool tudor_enroll_start(struct tudor_device *device, RECGUID guid, enum tudor_finger finger) {
    winmodule_set_cur(&tudor_adapter_dll->module);
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

static void enroll_cb(OVERLAPPED *ovlp, NTSTATUS status, tudor_async_res_t res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    HRESULT hres;
    bool success = false, done = true;

    if(status != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        goto exit;
    }

    //Follow https://docs.microsoft.com/en-us/windows/win32/secbiomet/adapter-workflow - WinBioEnrollCapture
    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(res->dev->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) done = false;
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(res->dev->pipeline, WINBIO_PURPOSE_ENROLL, 0, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) done = false;
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };

    log_debug("Updating enrollment...");
    if((hres = tudor_engine_adapter->UpdateEnrollment(res->dev->pipeline, &reject_detail)) != ERROR_SUCCESS && hres != WINBIO_I_MORE_DATA) {
        if(hres == WINBIO_E_BAD_CAPTURE) done = false;
        log_error("Error updating enrollment: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };

    success = true;
    done = hres != WINBIO_I_MORE_DATA;

    exit:;
    *res->args.enroll.done = done;
    async_complete_op(res, success);
}

bool tudor_enroll_capture(struct tudor_device *device, bool *done, tudor_async_res_t *res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    *res = NULL;
    HRESULT hres;

    *done = true;
    if(!device->enrolling) {
        log_error("Not currently enrolling a finger!");
        return false;
    }

    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_ENROLL, &ovlp);

    *res = async_new_res(device, ovlp);
    (*res)->args.enroll = (struct async_args_enroll) { .done = done };
    winio_set_overlapped_callback(ovlp, (winio_overlapped_cb_fnc*) enroll_cb, *res, true);
    return true;
}

bool tudor_enroll_commit(struct tudor_device *device, bool *is_duplicate) {
    winmodule_set_cur(&tudor_adapter_dll->module);
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

bool tudor_enroll_discard(struct tudor_device *device) {
    winmodule_set_cur(&tudor_adapter_dll->module);
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

static void verify_cb(OVERLAPPED *ovlp, NTSTATUS status, tudor_async_res_t res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    HRESULT hres;
    bool success = false, retry = false, matches = false;

    if(status != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        goto exit;
    }

    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(res->dev->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(res->dev->pipeline, WINBIO_PURPOSE_VERIFY, 0, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };

    log_debug("Verifying sample...");
    BOOLEAN is_match;
    UCHAR *payload_ptr, *hash_ptr;
    SIZE_T payload_size, hash_size;
    if((hres = tudor_engine_adapter->VerifyFeatureSet(res->dev->pipeline, &(WINBIO_IDENTITY) {
        .Type = WINBIO_ID_TYPE_GUID,
        .TemplateGuid = *(GUID*) &res->args.verify.guid
    }, (UCHAR) res->args.verify.finger, &is_match, &payload_ptr, &payload_size, &hash_ptr, &hash_size, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        if(hres == WINBIO_E_NO_MATCH) {
            success = true;
            matches = false;
            goto exit;
        }

        log_error("Error verifying sample: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };

    success = true;
    matches = is_match;

    exit:;
    *(res->args.verify.retry) = retry;
    *(res->args.verify.matches) = matches;
    async_complete_op(res, success);
}

bool tudor_verify(struct tudor_device *device, RECGUID guid, enum tudor_finger finger, bool *retry, bool *matches, tudor_async_res_t *res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    *res = NULL;
    HRESULT hres;

    *retry = false;
    *matches = false;
    if(device->enrolling) {
        log_error("Currently enrolling a finger!");
        return false;
    }

    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_VERIFY, &ovlp);

    *res = async_new_res(device, ovlp);
    (*res)->args.verify = (struct async_args_verify) { .retry = retry, .guid = guid, .finger = finger, .matches = matches };
    winio_set_overlapped_callback(ovlp, (winio_overlapped_cb_fnc*) verify_cb, *res, true);
    return true;
}

static void identify_cb(OVERLAPPED *ovlp, NTSTATUS status, tudor_async_res_t res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    HRESULT hres;
    bool success = false, retry = false, found_match = false;

    if(status != STATUS_SUCCESS) {
        log_error("Error starting capture: 0x%x!", status);
        goto exit;
    }

    ULONG reject_detail;
    if((hres = tudor_sensor_adapter->FinishCapture(res->dev->pipeline, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        log_error("Error finishing sensor capture: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };
    if((hres = tudor_sensor_adapter->PushDataToEngine(res->dev->pipeline, WINBIO_PURPOSE_IDENTIFY, 0, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        log_error("Error pushing sensor data to engine: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };

    log_debug("Identifying sample...");
    WINBIO_IDENTITY identity;
    UCHAR subfactor;
    UCHAR *payload_ptr, *hash_ptr;
    SIZE_T payload_size, hash_size;
    if((hres = tudor_engine_adapter->IdentifyFeatureSet(res->dev->pipeline, &identity, &subfactor, &payload_ptr, &payload_size, &hash_ptr, &hash_size, &reject_detail)) != ERROR_SUCCESS) {
        if(hres == WINBIO_E_BAD_CAPTURE) retry = true;
        if(hres == WINBIO_E_UNKNOWN_ID) {
            success = true;
            found_match = false;
            goto exit;
        }

        log_error("Error identifying sample: 0x%x! [reject detail 0x%x]", hres, reject_detail);
        goto exit;
    };
    if(identity.Type != WINBIO_ID_TYPE_GUID) {
        log_error("Invalid identification ID type: %d!", identity.Type);
        goto exit;
    }

    success = true;
    found_match = true;
    *(res->args.identify.guid) = *(RECGUID*) &identity.TemplateGuid;
    *(res->args.identify.finger) = (enum tudor_finger) subfactor;

    exit:;
    *(res->args.identify.retry) = retry;
    *(res->args.identify.found_match) = found_match;
    async_complete_op(res, success);
}

bool tudor_identify(struct tudor_device *device, bool *retry, bool *found_match, RECGUID *guid, enum tudor_finger *finger, tudor_async_res_t *res) {
    winmodule_set_cur(&tudor_adapter_dll->module);
    *res = NULL;
    HRESULT hres;

    *retry = false;
    *found_match = false;
    if(device->enrolling) {
        log_error("Currently enrolling a finger!");
        return false;
    }

    log_debug("Capturing sample...");
    OVERLAPPED *ovlp;
    WINBIO_CALL_PIPELINE(tudor_sensor_adapter->StartCapture, device->pipeline, WINBIO_PURPOSE_IDENTIFY, &ovlp);

    *res = async_new_res(device, ovlp);
    (*res)->args.identify = (struct async_args_identify) { .retry = retry, .found_match = found_match, .guid = guid, .finger = finger };
    winio_set_overlapped_callback(ovlp, (winio_overlapped_cb_fnc*) identify_cb, *res, true);
    return true;
}