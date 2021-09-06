#include <stdbool.h>
#include <Python.h>
#include "fpi-image.h"
#include "fpi-image-device.h"

const char *wrapper_src =
    "import tudor\n"
    "import usb.core\n"
    "import usb.util\n"
    "import os\n"
    "def open_device(bus, addr):\n"
    "    usbDev = usb.core.find(custom_match = lambda d: d.bus == bus and d.address == addr)\n"
    "    if usbDev == None: raise ValueError(\"USB device not found\")\n"
    "    comm = tudor.USBCommunication(usbDev)\n"
    "    err = None\n"
    "    sensor = None\n"
    "    for _ in range(4):\n"
    "        try:\n" 
    "            sensor = tudor.sensor.Sensor(comm)\n"
    "            break\n"
    "        except tudor.comm.CommandFailedException as e: err = e\n"
    "    else: raise err\n"
    "    with open(\"/etc/tudor/%s.pdata\" % sensor.id.hex(), \"rb\") as f: sensor.initialize(tudor.sensor.SensorPairingData.load(f))\n"
    "    return sensor\n"
    "def close_device(dev):\n"
    "    dev.uninitialize()\n"
    "    dev.comm.close()\n"
    "def read_image(dev):\n"
    "    dev.event_handler.wait_for_event([tudor.sensor.SensorEventType.FINGER_REMOVE])\n"
    "    img = dev.frame_capturer.capture_images(1)[0]\n"
    "    return img if img.enough_coverage else None\n"
;

static _Py_NO_RETURN void py_error(const char *msg) {
    if(PyErr_Occurred()) PyErr_Print();
    g_error("%s", msg);
}

struct _FpiDeviceTudor {
    FpImageDevice parent;

    PyThreadState *interpreter;
    PyObject *wrapper_module;
    PyObject *sensor_obj;

    GThread *img_thread;
};

G_DECLARE_FINAL_TYPE(FpiDeviceTudor, fpi_device_tudor, FPI, DEVICE_TUDOR, FpImageDevice)
G_DEFINE_TYPE(FpiDeviceTudor, fpi_device_tudor, FP_TYPE_IMAGE_DEVICE)

static const FpIdEntry id_table[] = {
    { .vid = 0x06cb, .pid = 0x00be },
    { .vid = 0, .pid = 0, .driver_data = 0}
};

static void create_device_obj(FpiDeviceTudor *dev, GUsbDevice *usb_dev, GError **error) {
    PyEval_AcquireThread(dev->interpreter);
        //Get open function
        PyObject *open_func = PyDict_GetItemString(dev->wrapper_module, "open_device");
        if(open_func == NULL) py_error("Couldn't get wrapper python module's 'open_device' function!");
        if(!PyCallable_Check(open_func)) g_error("Wrapper python module's 'open_device' function isn't callable!");

        //Open device
        dev->sensor_obj = PyObject_CallFunction(open_func, "bb", g_usb_device_get_bus(usb_dev), g_usb_device_get_address(usb_dev));
        if(dev->sensor_obj == NULL) {
            if(PyErr_Occurred() != NULL) PyErr_Print();
            PyErr_Clear();
            g_set_error(error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_GENERAL, "Couldn't open tudor device on bus %d address %d through wrapper python module! (did you setup pairing data?)", g_usb_device_get_bus(usb_dev), g_usb_device_get_address(usb_dev));
        }
    PyEval_ReleaseThread(dev->interpreter);
}

static void fpi_device_tudor_open(FpImageDevice *img_dev) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(img_dev);
    GError *error = NULL;

    g_assert(dev->sensor_obj == NULL);

    //Create device object
    create_device_obj(dev, fpi_device_get_usb_device(FP_DEVICE(dev)), &error);

    fpi_device_open_complete(FP_DEVICE(dev), error);
}

static void fpi_device_tudor_close(FpImageDevice *img_dev) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(img_dev);
    GError *error = NULL;

    g_assert(dev->sensor_obj != NULL);

    PyEval_AcquireThread(dev->interpreter);
        //Get close function
        PyObject *close_func = PyDict_GetItemString(dev->wrapper_module, "close_device");
        if(close_func == NULL) py_error("Couldn't get wrapper python module's 'close_device' function!");
        if(!PyCallable_Check(close_func)) g_error("Wrapper python module's 'close_device' function isn't callable!");

        //Close device object
        PyObject *close_ret = PyObject_CallFunction(close_func, "N", dev->sensor_obj);
        if(close_ret == NULL) {
            if(PyErr_Occurred() != NULL) PyErr_Print();
            PyErr_Clear();
            g_set_error(&error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_GENERAL, "Couldn't close tudor device through wrapper python module!");
        } else Py_DECREF(close_ret);
    PyEval_ReleaseThread(dev->interpreter);

    dev->sensor_obj = NULL;

    fpi_device_close_complete(FP_DEVICE(dev), error);
}

static gpointer img_thread_fnc(gpointer user) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(user);

    //Get read image function
    PyEval_AcquireThread(dev->interpreter);
        PyObject *read_img_func = PyDict_GetItemString(dev->wrapper_module, "read_image");
        if(read_img_func == NULL) py_error("Couldn't get wrapper python module's 'read_image' function!");
        if(!PyCallable_Check(read_img_func)) g_error("Wrapper python module's 'read_image' function isn't callable!");
    PyEval_ReleaseThread(dev->interpreter);

    //Finish activation
    {
        FpiImageDeviceState state;
        g_object_get(dev, "fpi-image-device-state", &state, NULL);
        if(state == FPI_IMAGE_DEVICE_STATE_ACTIVATING) fpi_image_device_activate_complete(FP_IMAGE_DEVICE(dev), NULL);
    }

    while(dev->img_thread != NULL) {
        g_autoptr(FpImage) img = NULL;
        GError *error = NULL;

        PyEval_AcquireThread(dev->interpreter);
            //Read image
            PyObject *py_img = PyObject_CallFunction(read_img_func, "O", dev->sensor_obj);
            if(dev->img_thread != NULL) {
                if(py_img == NULL && (PyErr_Occurred() == NULL || !PyErr_ExceptionMatches(PyExc_InterruptedError))) {
                    if(PyErr_Occurred() != NULL) PyErr_Print();
                    g_set_error(&error, FP_DEVICE_ERROR, FP_DEVICE_ERROR_GENERAL, "Couldn't read image from tudor device through wrapper python module!");
                } else if(py_img != NULL) {
                    if(py_img == Py_None) {
                        //Retry capture
                        fpi_image_device_retry_scan(FP_IMAGE_DEVICE(dev), FP_DEVICE_RETRY_GENERAL);
                        img = NULL;
                    } else {
                        //Get image width & height
                        PyObject *py_width = PyObject_GetAttrString(py_img, "width");
                        if(py_width == NULL) py_error("Couldn't get python image's width!");
                        long width = PyLong_AsLong(py_width);
                        Py_DECREF(py_width);
                        
                        PyObject *py_height = PyObject_GetAttrString(py_img, "height");
                        if(py_height == NULL) py_error("Couldn't get python image's height!");
                        long height = PyLong_AsLong(py_height);
                        Py_DECREF(py_height);

                        //Create new image
                        img = fp_image_new(width, height);
                        guint8 *data = (guint8*) img->data;

                        //Copy data
                        PyObject *py_data = PyObject_GetAttrString(py_img, "data");
                        if(py_data == NULL) py_error("Couldn't get python image's data!");
                        if(!PyList_Check(py_data)) g_error("Python image data isn't a list!");
                        for(long x = 0; x < width; x++) {
                            PyObject *column = PyList_GetItem(py_data, x);
                            if(column == NULL) py_error("Couldn't get python image column!");
                            if(!PyList_Check(column)) g_error("Python image column isn't a list!");

                            for(long y = 0; y < height; y++) {
                                PyObject *py_px = PyList_GetItem(column, y);
                                if(py_px == NULL) py_error("Couldn't get python image pixel!");
                                if(!PyLong_Check(py_px)) g_error("Python image pixel isn't a long!");
                                data[x + y * width] = (uint8_t) PyLong_AsLong(py_px);
                            }
                        }
                        Py_DECREF(py_data);
                    }
                }
            }
            Py_XDECREF(py_img);
        PyEval_ReleaseThread(dev->interpreter);

        //Handle errors
        if(error != NULL) {
            fpi_image_device_session_error(FP_IMAGE_DEVICE(dev), error);
            break;
        }

        //Pass image to libfprint
        if(img == NULL) continue;

        FpiImageDeviceState state;
        g_object_get(dev, "fpi-image-device-state", &state, NULL);
        if(state == FPI_IMAGE_DEVICE_STATE_AWAIT_FINGER_ON || state == FPI_IMAGE_DEVICE_STATE_CAPTURE) {
            fpi_image_device_report_finger_status(FP_IMAGE_DEVICE(dev), true);
            fpi_image_device_image_captured(FP_IMAGE_DEVICE(dev), g_steal_pointer(&img));
            fpi_image_device_report_finger_status(FP_IMAGE_DEVICE(dev), false);
        }
    }

    //Clear exceptions
    PyEval_AcquireThread(dev->interpreter);
        PyThreadState_SetAsyncExc(dev->interpreter->thread_id, NULL);
        PyErr_Clear();
    PyEval_ReleaseThread(dev->interpreter);

    //Finish deactivation
    {
        FpiImageDeviceState state;
        g_object_get(dev, "fpi-image-device-state", &state, NULL);
        if(state == FPI_IMAGE_DEVICE_STATE_DEACTIVATING) fpi_image_device_deactivate_complete(FP_IMAGE_DEVICE(dev), NULL);
    }

    return NULL;
}

static void fpi_device_tudor_activate(FpImageDevice *img_dev) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(img_dev);

    g_assert(dev->img_thread == NULL);

    //Start image thread
    dev->img_thread = g_thread_new("tudor_capture", img_thread_fnc, dev);
}

static void fpi_device_tudor_deactivate(FpImageDevice *img_dev) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(img_dev);

    g_assert(dev->img_thread != NULL);

    PyEval_AcquireThread(dev->interpreter);
        //Send exception
        //Internally, the image capture thread is the same python thread
        g_assert(PyThreadState_SetAsyncExc(dev->interpreter->thread_id, PyExc_InterruptedError) > 0);

        //Cancel image thread
        g_thread_unref(dev->img_thread);
        dev->img_thread = NULL;
    PyEval_ReleaseThread(dev->interpreter);
}

static int num_python_instances = 0;
static PyThreadState *python_main_thread;

extern unsigned char ___wrapper_py[];
extern unsigned int ___wrapper_py_len;

static void fpi_device_tudor_init(FpiDeviceTudor *dev) {
    //Initialize python interpreter
    if(num_python_instances++ <= 0) {
        Py_Initialize();
        python_main_thread = PyEval_SaveThread();
    }

    //Load & compile wrapper module
    PyEval_RestoreThread(python_main_thread);
        dev->interpreter = Py_NewInterpreter();
        if(dev->interpreter == NULL) py_error("Couldn't create python interpreter!");

        PyThreadState *ostate = PyThreadState_Swap(dev->interpreter);
            PyObject *wrapper_code = Py_CompileString(wrapper_src, "wrapper.py", Py_file_input);
            if(wrapper_code == NULL) py_error("Couldn't compile wrapper python module!");

            PyObject *module = PyImport_ExecCodeModule("wrapper", wrapper_code);
            if(module == NULL) py_error("Couldn't load wrapper python module!");
            dev->wrapper_module = PyModule_GetDict(module);

            Py_DECREF(module);
            Py_DECREF(wrapper_code);
        PyThreadState_Swap(ostate);
    python_main_thread = PyEval_SaveThread();

    dev->sensor_obj = NULL;
    dev->img_thread = NULL;
}

static void fpi_device_tudor_dispose(GObject* obj) {
    FpiDeviceTudor *dev = FPI_DEVICE_TUDOR(obj);

    g_assert(dev->sensor_obj == NULL);
    g_assert(dev->wrapper_module != NULL);
    g_assert(dev->img_thread == NULL);

    //Finalize python wrapper module & interpreter
    PyEval_RestoreThread(python_main_thread);
        PyThreadState *ostate = PyThreadState_Swap(dev->interpreter);
        Py_DECREF(dev->wrapper_module);
        dev->wrapper_module = NULL;
        Py_EndInterpreter(dev->interpreter);
        PyThreadState_Swap(ostate);
    python_main_thread = PyEval_SaveThread();

    if(--num_python_instances <= 0) {
        PyEval_RestoreThread(python_main_thread);
        Py_Finalize();
        python_main_thread = NULL;
    }

    G_OBJECT_CLASS(fpi_device_tudor_parent_class)->dispose(obj);
}

static void fpi_device_tudor_class_init(FpiDeviceTudorClass* klass) {
    //Init object info
    G_OBJECT_CLASS(klass)->dispose = fpi_device_tudor_dispose;

    //Init device info
    FpDeviceClass *dev_class = FP_DEVICE_CLASS(klass);
    dev_class->id = "tudor";
    dev_class->full_name = "Synaptics Tudor";
    dev_class->type = FP_DEVICE_TYPE_USB;
    dev_class->id_table = id_table;
    dev_class->scan_type = FP_SCAN_TYPE_PRESS;

    //Init device callbacks
    FpImageDeviceClass *img_dev_class = FP_IMAGE_DEVICE_CLASS(klass);
    img_dev_class->img_open = fpi_device_tudor_open;
    img_dev_class->img_close = fpi_device_tudor_close;
    img_dev_class->activate = fpi_device_tudor_activate;
    img_dev_class->deactivate = fpi_device_tudor_deactivate;
}