#include "internal.h"

typedef enum {
  WdfPowerDeviceInvalid,
  WdfPowerDeviceD0,
  WdfPowerDeviceD1,
  WdfPowerDeviceD2,
  WdfPowerDeviceD3,
  WdfPowerDeviceD3Final,
  WdfPowerDevicePrepareForHibernation,
  WdfPowerDeviceMaximum
} WDF_POWER_DEVICE_STATE;

typedef NTSTATUS __winfnc WDF_DEVICE_D0_ENTRY(WDFOBJECT Device, WDF_POWER_DEVICE_STATE PreviousState);
typedef NTSTATUS __winfnc WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED(WDFOBJECT Device, WDF_POWER_DEVICE_STATE PreviousState);
typedef NTSTATUS __winfnc WDF_DEVICE_D0_EXIT(WDFOBJECT Device, WDF_POWER_DEVICE_STATE TargetState);
typedef NTSTATUS __winfnc WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED(WDFOBJECT Device, WDF_POWER_DEVICE_STATE TargetState);
typedef NTSTATUS __winfnc WDF_DEVICE_PREPARE_HARDWARE(WDFOBJECT Device, WDFOBJECT ResourcesRaw, WDFOBJECT ResourcesTranslated);
typedef NTSTATUS __winfnc WDF_DEVICE_RELEASE_HARDWARE(WDFOBJECT Device, WDFOBJECT ResourcesTranslated);
typedef void __winfnc WDF_DEVICE_SELF_MANAGED_IO_CLEANUP(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_SELF_MANAGED_IO_FLUSH(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_SELF_MANAGED_IO_INIT(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_SELF_MANAGED_IO_SUSPEND(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_SELF_MANAGED_IO_RESTART(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_QUERY_STOP(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_QUERY_REMOVE(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_SURPRISE_REMOVAL(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_USAGE_NOTIFICATION(WDFOBJECT Device, ULONG NotificationType, BOOL IsInNotificationPath);
typedef void __winfnc WDF_DEVICE_RELATIONS_QUERY(WDFOBJECT Device, ULONG RelationType);
typedef NTSTATUS __winfnc WDF_DEVICE_USAGE_NOTIFICATION_EX(WDFOBJECT Device, ULONG NotificationType, BOOL IsInNotificationPath);

typedef struct {
    ULONG Size;
    WDF_DEVICE_D0_ENTRY *EvtDeviceD0Entry;
    WDF_DEVICE_D0_ENTRY_POST_INTERRUPTS_ENABLED *EvtDeviceD0EntryPostInterruptsEnabled;
    WDF_DEVICE_D0_EXIT *EvtDeviceD0Exit;
    WDF_DEVICE_D0_EXIT_PRE_INTERRUPTS_DISABLED *EvtDeviceD0ExitPreInterruptsDisabled;
    WDF_DEVICE_PREPARE_HARDWARE *EvtDevicePrepareHardware;
    WDF_DEVICE_RELEASE_HARDWARE *EvtDeviceReleaseHardware;
    WDF_DEVICE_SELF_MANAGED_IO_CLEANUP *EvtDeviceSelfManagedIoCleanup;
    WDF_DEVICE_SELF_MANAGED_IO_FLUSH *EvtDeviceSelfManagedIoFlush;
    WDF_DEVICE_SELF_MANAGED_IO_INIT *EvtDeviceSelfManagedIoInit;
    WDF_DEVICE_SELF_MANAGED_IO_SUSPEND *EvtDeviceSelfManagedIoSuspend;
    WDF_DEVICE_SELF_MANAGED_IO_RESTART *EvtDeviceSelfManagedIoRestart;
    WDF_DEVICE_SURPRISE_REMOVAL *EvtDeviceSurpriseRemoval;
    WDF_DEVICE_QUERY_REMOVE *EvtDeviceQueryRemove;
    WDF_DEVICE_QUERY_STOP *EvtDeviceQueryStop;
    WDF_DEVICE_USAGE_NOTIFICATION *EvtDeviceUsageNotification;
    WDF_DEVICE_RELATIONS_QUERY *EvtDeviceRelationsQuery;
    WDF_DEVICE_USAGE_NOTIFICATION_EX *EvtDeviceUsageNotificationEx;
} WDF_PNPPOWER_EVENT_CALLBACKS;

typedef NTSTATUS __winfnc WDF_DEVICE_ARM_WAKE_FROM_S0(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_ARM_WAKE_FROM_SX(WDFOBJECT Device);
typedef NTSTATUS __winfnc WDF_DEVICE_ARM_WAKE_FROM_SX_WITH_REASON(WDFOBJECT Device, BOOL DeviceWakeEnabled, BOOL ChildrenArmedForWake);
typedef void __winfnc WDF_DEVICE_DISARM_WAKE_FROM_S0(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_DISARM_WAKE_FROM_SX(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_WAKE_FROM_S0_TRIGGERED(WDFOBJECT Device);
typedef void __winfnc WDF_DEVICE_WAKE_FROM_SX_TRIGGERED(WDFOBJECT Device);

typedef struct {
    ULONG Size;
    WDF_DEVICE_ARM_WAKE_FROM_S0 *EvtDeviceArmWakeFromS0;
    WDF_DEVICE_DISARM_WAKE_FROM_S0 *EvtDeviceDisarmWakeFromS0;
    WDF_DEVICE_WAKE_FROM_S0_TRIGGERED *EvtDeviceWakeFromS0Triggered;
    WDF_DEVICE_ARM_WAKE_FROM_SX *EvtDeviceArmWakeFromSx;
    WDF_DEVICE_DISARM_WAKE_FROM_SX *EvtDeviceDisarmWakeFromSx;
    WDF_DEVICE_WAKE_FROM_SX_TRIGGERED *EvtDeviceWakeFromSxTriggered;
    WDF_DEVICE_ARM_WAKE_FROM_SX_WITH_REASON *EvtDeviceArmWakeFromSxWithReason;
} WDF_POWER_POLICY_EVENT_CALLBACKS;

typedef struct {
    ULONG AddressDescriptionSize;
} WDF_CHILD_ADDRESS_DESCRIPTION_HEADER;

struct wdf_device_init {
    struct winwdf_driver *driver;
    HANDLE reg_key;
    libusb_device_handle *usb_dev;

    WDF_PNPPOWER_EVENT_CALLBACKS *pnp_callbacks;
    WDF_POWER_POLICY_EVENT_CALLBACKS *power_callbacks;

    WDF_FILEOBJECT_CONFIG *fs_cfg;
    WDF_OBJECT_ATTRIBUTES *fs_obj_attrs;

    struct winwdf_device **out_dev;
};

struct wdf_device_init *wdf_create_device_init(struct winwdf_driver *driver, HANDLE reg_key, libusb_device_handle *usb_dev, struct winwdf_device **out_dev) {
    struct wdf_device_init *dev_init = (struct wdf_device_init*) malloc(sizeof(struct wdf_device_init));
    if(!dev_init) { perror("Couldn't allocate device init object memory"); abort(); }

    //Initialize the object
    *dev_init = (struct wdf_device_init) {0};
    dev_init->driver = driver;
    dev_init->reg_key = reg_key;
    dev_init->usb_dev = usb_dev;
    dev_init->out_dev = out_dev;

    return dev_init;
}

struct dev_queue_node {
    struct dev_queue_node *next, *prev;
    struct winwdf_queue *queue;
};

struct winwdf_device {
    struct wdf_object object;
    struct winwdf_driver *driver;
    bool is_dying;
    HANDLE reg_key;

    //Callbacks
    WDF_PNPPOWER_EVENT_CALLBACKS *pnp_callbacks;
    WDF_POWER_POLICY_EVENT_CALLBACKS *power_callbacks;

    //FS
    WDF_FILEOBJECT_CONFIG *fs_cfg;
    WDF_OBJECT_ATTRIBUTES *fs_obj_attrs;

    //Queues
    pthread_mutex_t queues_lock;
    struct dev_queue_node *queues_head;
    struct winwdf_queue *create_queue, *read_queue, *write_queue, *devctrl_queue, *devctrl_int_queue;

    //USB
    struct wdf_usb_device *usb_dev;
    libusb_device_handle *libusb_dev;
};

void winwdf_remove_device(struct winwdf_device *dev) {
    winwdf_destroy_object(&dev->object);
}

static void device_destr(struct winwdf_device *dev) {
    NTSTATUS status = 0;
    dev->is_dying = true;

    //Call callbacks I
    if(dev->pnp_callbacks) {
        log_debug("Calling WDF device removal callbacks...");
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceSelfManagedIoSuspend, &dev->object)) != 0) goto cbErr;
    }

    //Destroy queues
    cant_fail_ret(pthread_mutex_lock(&dev->queues_lock));
    while(dev->queues_head) {
        struct dev_queue_node *q = dev->queues_head;
        dev->queues_head = q->next;
        winwdf_destroy_object((WDFOBJECT) q->queue);
        free(q);
    }
    cant_fail_ret(pthread_mutex_unlock(&dev->queues_lock));
    cant_fail_ret(pthread_mutex_destroy(&dev->queues_lock));

    //Call callbacks II
    if(dev->pnp_callbacks) {
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceD0ExitPreInterruptsDisabled, &dev->object, WdfPowerDeviceD3)) != 0) goto cbErr;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceD0Exit, &dev->object, WdfPowerDeviceD3)) != 0) goto cbErr;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceReleaseHardware, &dev->object, NULL)) != 0) goto cbErr;
        WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceSelfManagedIoFlush, &dev->object);
        WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceSelfManagedIoCleanup, &dev->object);

        cbErr:;
        if(status) {
            log_error("Error in WDF device removal callbacks: 0x%x!", status);
            abort();
        }
    }

    //Free memory
    if(dev->usb_dev) winwdf_destroy_object((WDFOBJECT) dev->usb_dev);
    wdf_cleanup_obj(&dev->object);
    free(dev);
}

static void device_call_cbs(struct winwdf_device *dev) {
    //Call callbacks
    if(dev->pnp_callbacks) {
        log_debug("Calling WDF device attachment callbacks...");

        NTSTATUS status = 0;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDevicePrepareHardware, &dev->object, NULL, NULL)) != 0) goto cbErr;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceD0Entry, &dev->object, WdfPowerDeviceD3Final)) != 0) goto cbErr;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceD0EntryPostInterruptsEnabled, &dev->object, WdfPowerDeviceD3Final)) != 0) goto cbErr;
        if((status = WIN_CALL_CALLBACK(dev->pnp_callbacks->EvtDeviceSelfManagedIoInit, &dev->object)) != 0) goto cbErr;

        cbErr:;
        if(status) {
            log_error("Error in WDF device attachment callbacks: 0x%x!", status);
            abort();
        }
    }
}

HANDLE wdf_get_device_reg_key(struct winwdf_device *dev) { return dev->reg_key; }

void wdf_add_device_queue(struct winwdf_device *dev, struct winwdf_queue* queue) {
    //Allocate and insert queue node
    struct dev_queue_node *q = (struct dev_queue_node*) malloc(sizeof(struct dev_queue_node));
    q->queue = queue;

    cant_fail_ret(pthread_mutex_lock(&dev->queues_lock));
    q->prev = NULL;
    q->next = dev->queues_head;
    if(dev->queues_head) dev->queues_head->prev = q;
    dev->queues_head = q;
    cant_fail_ret(pthread_mutex_unlock(&dev->queues_lock));
}

void wdf_remove_device_queue(struct winwdf_device *dev, struct winwdf_queue* queue) {
    //Find the queue node
    if(!dev->is_dying) cant_fail_ret(pthread_mutex_lock(&dev->queues_lock));
    for(struct dev_queue_node *q = dev->queues_head; q; q = q->next) {
        if(q->queue != queue) continue;

        //Remove the queue node
        if(q->prev) q->prev->next = q->next;
        else dev->queues_head = q->next;
        if(q->next) q->next->prev = q->prev;
        free(q);
    }
    if(!dev->is_dying) cant_fail_ret(pthread_mutex_unlock(&dev->queues_lock));
}

struct winwdf_queue *wdf_get_dispatch_queue(struct winwdf_device *dev, enum wdf_queue_req_type req_type) {
    struct winwdf_queue *q = NULL;
    cant_fail_ret(pthread_mutex_lock(&dev->queues_lock));
    switch(req_type) {
        case WDF_QUEUE_REQ_CREATE: q = dev->create_queue; break;
        case WDF_QUEUE_REQ_READ: q = dev->read_queue; break;
        case WDF_QUEUE_REQ_WRITE: q = dev->write_queue; break;
        case WDF_QUEUE_REQ_DEVCTRL: q = dev->devctrl_queue; break;
        case WDF_QUEUE_REQ_DEVCTRL_INT: q = dev->devctrl_int_queue; break;
    }
    cant_fail_ret(pthread_mutex_unlock(&dev->queues_lock));
    return q;
}

void wdf_configure_device_dispatch(struct winwdf_device *dev, struct winwdf_queue* queue, enum wdf_queue_req_type req_type) {
    cant_fail_ret(pthread_mutex_lock(&dev->queues_lock));
    switch(req_type) {
        case WDF_QUEUE_REQ_CREATE: dev->create_queue = queue; break;
        case WDF_QUEUE_REQ_READ: dev->read_queue = queue; break;
        case WDF_QUEUE_REQ_WRITE: dev->write_queue = queue; break;
        case WDF_QUEUE_REQ_DEVCTRL: dev->devctrl_queue = queue; break;
        case WDF_QUEUE_REQ_DEVCTRL_INT: dev->devctrl_int_queue = queue; break;
    }
    cant_fail_ret(pthread_mutex_unlock(&dev->queues_lock));
}

libusb_device_handle *wdf_get_libusb_device(struct winwdf_device *dev) { return dev->libusb_dev; }

struct wdf_usb_device * wdf_get_usb_device(struct winwdf_device *dev) { return dev->usb_dev; }
void wdf_set_usb_device(struct winwdf_device *dev, struct wdf_usb_device *usb_dev) { dev->usb_dev = usb_dev; }

WDF_FILEOBJECT_CONFIG *wdf_get_fs_config(struct winwdf_device *dev) { return dev->fs_cfg; }
WDF_OBJECT_ATTRIBUTES *wdf_get_fs_obj_attrs(struct winwdf_device *dev) { return dev->fs_obj_attrs; }

__winfnc void WdfDeviceInitSetPowerPolicyOwnership(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init *dev_init, BOOL IsPowerPolicyOwner) {}
WDFFUNC(WdfDeviceInitSetPowerPolicyOwnership, 21)

__winfnc void WdfDeviceInitSetIoType(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init *dev_init, ULONG IoType) {}
WDFFUNC(WdfDeviceInitSetIoType, 22)

__winfnc void WdfDeviceInitSetPnpPowerEventCallbacks(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init *dev_init, WDF_PNPPOWER_EVENT_CALLBACKS *callbacks) {
    free(dev_init->pnp_callbacks);
    dev_init->pnp_callbacks = malloc(callbacks->Size);
    if(!dev_init->pnp_callbacks) { perror("Error allocating WDF device PNP callbacks struct"); abort(); }
    memcpy(dev_init->pnp_callbacks, callbacks, callbacks->Size);
}
WDFFUNC(WdfDeviceInitSetPnpPowerEventCallbacks, 19)

__winfnc void WdfDeviceInitSetPowerPolicyEventCallbacks(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init *dev_init, WDF_POWER_POLICY_EVENT_CALLBACKS *callbacks) {
    free(dev_init->power_callbacks);
    dev_init->power_callbacks = malloc(callbacks->Size);
    if(!dev_init->power_callbacks) { perror("Error allocating WDF device power callbacks struct"); abort(); }
    memcpy(dev_init->power_callbacks, callbacks, callbacks->Size);
}
WDFFUNC(WdfDeviceInitSetPowerPolicyEventCallbacks, 20)

__winfnc void WdfDeviceInitSetFileObjectConfig(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init *dev_init, WDF_FILEOBJECT_CONFIG *config, WDF_OBJECT_ATTRIBUTES *obj_attrs) {
    free(dev_init->fs_cfg);
    free(dev_init->fs_obj_attrs);
    dev_init->fs_cfg = malloc(config->Size);
    dev_init->fs_obj_attrs = obj_attrs ? malloc(obj_attrs->Size) : NULL;
    memcpy(dev_init->fs_cfg, config, config->Size);
    if(obj_attrs) memcpy(dev_init->fs_obj_attrs, obj_attrs, obj_attrs->Size);
}
WDFFUNC(WdfDeviceInitSetFileObjectConfig, 23)

__winfnc NTSTATUS WdfDeviceCreate(WDF_DRIVER_GLOBALS *globals, struct wdf_device_init **dev_init, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    //Create a device object
    struct winwdf_device *dev = (struct winwdf_device*) malloc(sizeof(struct winwdf_device));
    if(!dev) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &dev->object, (wdf_obj_destr_fnc*) device_destr, obj_attrs);
    dev->driver = (*dev_init)->driver;
    dev->is_dying = false;
    dev->reg_key = (*dev_init)->reg_key;

    dev->pnp_callbacks = (*dev_init)->pnp_callbacks;
    if(dev->pnp_callbacks && dev->pnp_callbacks->Size < sizeof(WDF_PNPPOWER_EVENT_CALLBACKS)) { free(dev->pnp_callbacks); dev->pnp_callbacks = NULL; }

    dev->power_callbacks = (*dev_init)->power_callbacks;
    if(dev->power_callbacks && dev->power_callbacks->Size < sizeof(WDF_POWER_POLICY_EVENT_CALLBACKS)) { free(dev->power_callbacks); dev->power_callbacks = NULL; }

    dev->fs_cfg = (*dev_init)->fs_cfg;
    dev->fs_obj_attrs = (*dev_init)->fs_obj_attrs;

    cant_fail_ret(pthread_mutex_init(&dev->queues_lock, NULL));
    dev->queues_head = NULL;
    dev->create_queue = dev->read_queue = dev->write_queue = dev->devctrl_queue = dev->devctrl_int_queue = NULL;

    dev->usb_dev = NULL;
    dev->libusb_dev = (*dev_init)->usb_dev;

    //Enqueue callback call
    wdf_evtqueue_enqueue(&dev->object, (wdf_evtqueue_action_fnc*) device_call_cbs);

    //Cleanup
    *out = &dev->object;
    if((*dev_init)->out_dev) *(*dev_init)->out_dev = dev;
    free(*dev_init);
    *dev_init = NULL;
    return EXIT_SUCCESS;
}
WDFFUNC(WdfDeviceCreate, 25)

__winfnc NTSTATUS WdfDeviceAssignS0IdleSettings(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, void *settings) { return STATUS_SUCCESS; }
WDFFUNC(WdfDeviceAssignS0IdleSettings, 16)

__winfnc NTSTATUS WdfDeviceAssignSxWakeSettings(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, void *settings) { return STATUS_SUCCESS; }
WDFFUNC(WdfDeviceAssignSxWakeSettings, 17)

__winfnc ULONG WdfDeviceGetSystemPowerAction(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj) { return 0; /* PowerActionNone */ }
WDFFUNC(WdfDeviceGetSystemPowerAction, 41)

__winfnc void WdfDeviceStopIdleNoTrack(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj) {}
WDFFUNC(WdfDeviceStopIdleNoTrack, 88)

__winfnc void WdfDeviceResumeIdleNoTrack(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj) {}
WDFFUNC(WdfDeviceResumeIdleNoTrack, 89)

__winfnc NTSTATUS WdfDeviceStopIdleActual(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, BOOLEAN wait_d0) { return STATUS_SUCCESS; }
WDFFUNC(WdfDeviceStopIdleActual, 248)

__winfnc void WdfDeviceResumeIdleActual(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj) {}
WDFFUNC(WdfDeviceResumeIdleActual, 249)

__winfnc NTSTATUS WdfDeviceCreateDeviceInterface(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, const GUID *interface_guid, const UNICODE_STRING *ref_str) { return STATUS_SUCCESS; }
WDFFUNC(WdfDeviceCreateDeviceInterface, 27)

__winfnc void WdfDeviceSetDeviceInterfaceState(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, const GUID *interface_guid, const UNICODE_STRING *ref_str, BOOLEAN is_enabled) {}
WDFFUNC(WdfDeviceSetDeviceInterfaceState, 28)

__winfnc NTSTATUS WdfDeviceRetrieveDeviceInterfaceString(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device_obj, const GUID *interface_guid, const UNICODE_STRING *ref_str, WDFOBJECT str) {
    char buf[64];
    sprintf(buf, "IF_%08lx-%04lx-%04lx-%04lx-%012lx", (uint64_t) interface_guid->PartA, (uint64_t) interface_guid->PartB, (uint64_t) interface_guid->PartC, (uint64_t) interface_guid->PartD, (uint64_t) interface_guid->PartE);
    wdf_set_string((struct wdf_string*) str, buf);
    return STATUS_SUCCESS;
}
WDFFUNC(WdfDeviceRetrieveDeviceInterfaceString, 29)