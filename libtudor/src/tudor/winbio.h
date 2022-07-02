#ifndef LIBTUDOR_TUDOR_WINBIO_H
#define LIBTUDOR_TUDOR_WINBIO_H

#include "winapi/windows.h"

#define WINBIO_E_UNSUPPORTED_FACTOR 0x80098001L
#define WINBIO_E_INVALID_UNIT 0x80098002L
#define WINBIO_E_UNKNOWN_ID 0x80098003L
#define WINBIO_E_CANCELED 0x80098004L
#define WINBIO_E_NO_MATCH 0x80098005L
#define WINBIO_E_CAPTURE_ABORTED 0x80098006L
#define WINBIO_E_ENROLLMENT_IN_PROGRESS 0x80098007L
#define WINBIO_E_BAD_CAPTURE 0x80098008L
#define WINBIO_E_INVALID_CONTROL_CODE 0x80098009L
#define WINBIO_E_DATA_COLLECTION_IN_PROGRESS 0x8009800BL
#define WINBIO_E_UNSUPPORTED_DATA_FORMAT 0x8009800CL
#define WINBIO_E_UNSUPPORTED_DATA_TYPE 0x8009800DL
#define WINBIO_E_UNSUPPORTED_PURPOSE 0x8009800EL
#define WINBIO_E_INVALID_DEVICE_STATE 0x8009800FL
#define WINBIO_E_DEVICE_BUSY 0x80098010L
#define WINBIO_E_DATABASE_CANT_CREATE 0x80098011L
#define WINBIO_E_DATABASE_CANT_OPEN 0x80098012L
#define WINBIO_E_DATABASE_CANT_CLOSE 0x80098013L
#define WINBIO_E_DATABASE_CANT_ERASE 0x80098014L
#define WINBIO_E_DATABASE_CANT_FIND 0x80098015L
#define WINBIO_E_DATABASE_ALREADY_EXISTS 0x80098016L
#define WINBIO_E_DATABASE_FULL 0x80098018L
#define WINBIO_E_DATABASE_LOCKED 0x80098019L
#define WINBIO_E_DATABASE_CORRUPTED 0x8009801AL
#define WINBIO_E_DATABASE_NO_SUCH_RECORD 0x8009801BL
#define WINBIO_E_DUPLICATE_ENROLLMENT 0x8009801CL
#define WINBIO_E_DATABASE_READ_ERROR 0x8009801DL
#define WINBIO_E_DATABASE_WRITE_ERROR 0x8009801EL
#define WINBIO_E_DATABASE_NO_RESULTS 0x8009801FL
#define WINBIO_E_DATABASE_NO_MORE_RECORDS 0x80098020L
#define WINBIO_E_DATABASE_EOF 0x80098021L
#define WINBIO_E_DATABASE_BAD_INDEX_VECTOR 0x80098022L
#define WINBIO_E_INCORRECT_BSP 0x80098024L
#define WINBIO_E_INCORRECT_SENSOR_POOL 0x80098025L
#define WINBIO_E_NO_CAPTURE_DATA 0x80098026L
#define WINBIO_E_INVALID_SENSOR_MODE 0x80098027L
#define WINBIO_E_LOCK_VIOLATION 0x8009802AL
#define WINBIO_E_DUPLICATE_TEMPLATE 0x8009802BL
#define WINBIO_E_INVALID_OPERATION 0x8009802CL
#define WINBIO_E_SESSION_BUSY 0x8009802DL
#define WINBIO_E_CRED_PROV_DISABLED 0x80098030L
#define WINBIO_E_CRED_PROV_NO_CREDENTIAL 0x80098031L
#define WINBIO_E_DISABLED 0x80098032L
#define WINBIO_E_CONFIGURATION_FAILURE 0x80098033L
#define WINBIO_E_SENSOR_UNAVAILABLE 0x80098034L
#define WINBIO_E_SAS_ENABLED 0x80098035L
#define WINBIO_E_DEVICE_FAILURE 0x80098036L
#define WINBIO_E_FAST_USER_SWITCH_DISABLED 0x80098037L
#define WINBIO_E_NOT_ACTIVE_CONSOLE 0x80098038L
#define WINBIO_E_EVENT_MONITOR_ACTIVE 0x80098039L
#define WINBIO_E_INVALID_PROPERTY_TYPE 0x8009803AL
#define WINBIO_E_INVALID_PROPERTY_ID 0x8009803BL
#define WINBIO_E_UNSUPPORTED_PROPERTY 0x8009803CL
#define WINBIO_E_ADAPTER_INTEGRITY_FAILURE 0x8009803DL
#define WINBIO_E_INCORRECT_SESSION_TYPE 0x8009803EL
#define WINBIO_E_SESSION_HANDLE_CLOSED 0x8009803FL
#define WINBIO_E_DEADLOCK_DETECTED 0x80098040L
#define WINBIO_E_NO_PREBOOT_IDENTITY 0x80098041L
#define WINBIO_E_MAX_ERROR_COUNT_EXCEEDED 0x80098042L
#define WINBIO_E_AUTO_LOGON_DISABLED 0x80098043L
#define WINBIO_E_INVALID_TICKET 0x80098044L
#define WINBIO_E_TICKET_QUOTA_EXCEEDED 0x80098045L
#define WINBIO_E_DATA_PROTECTION_FAILURE 0x80098046L
#define WINBIO_E_CRED_PROV_SECURITY_LOCKOUT 0x80098047L
#define WINBIO_E_UNSUPPORTED_POOL_TYPE 0x80098048L
#define WINBIO_E_SELECTION_REQUIRED 0x80098049L
#define WINBIO_E_PRESENCE_MONITOR_ACTIVE 0x8009804AL
#define WINBIO_E_INVALID_SUBFACTOR 0x8009804BL
#define WINBIO_E_INVALID_CALIBRATION_FORMAT_ARRAY 0x8009804CL
#define WINBIO_E_NO_SUPPORTED_CALIBRATION_FORMAT 0x8009804DL
#define WINBIO_E_UNSUPPORTED_SENSOR_CALIBRATION_FORMAT 0x8009804EL
#define WINBIO_E_CALIBRATION_BUFFER_TOO_SMALL 0x8009804FL
#define WINBIO_E_CALIBRATION_BUFFER_TOO_LARGE 0x80098050L
#define WINBIO_E_CALIBRATION_BUFFER_INVALID 0x80098051L
#define WINBIO_E_INVALID_KEY_IDENTIFIER 0x80098052L
#define WINBIO_E_KEY_CREATION_FAILED 0x80098053L
#define WINBIO_E_KEY_IDENTIFIER_BUFFER_TOO_SMALL 0x80098054L
#define WINBIO_E_PROPERTY_UNAVAILABLE 0x80098055L
#define WINBIO_I_MORE_DATA 0x00090001L
#define WINBIO_I_EXTENDED_STATUS_INFORMATION 0x00090002L

#define WINBIO_FP_TOO_HIGH 0x00000001L
#define WINBIO_FP_TOO_LOW 0x00000002L
#define WINBIO_FP_TOO_LEFT 0x00000003L
#define WINBIO_FP_TOO_RIGHT 0x00000004L
#define WINBIO_FP_TOO_FAST 0x00000005L
#define WINBIO_FP_TOO_SLOW 0x00000006L
#define WINBIO_FP_POOR_QUALITY 0x00000007L
#define WINBIO_FP_TOO_SKEWED 0x00000008L
#define WINBIO_FP_TOO_SHORT 0x00000009L
#define WINBIO_FP_MERGE_FAILURE 0x0000000AL

#define WINBIO_SENSOR_ACCEPT 1
#define WINBIO_SENSOR_REJECT 2
#define WINBIO_SENSOR_READY 3
#define WINBIO_SENSOR_BUSY 4
#define WINBIO_SENSOR_NOT_CALIBRATED 5
#define WINBIO_SENSOR_FAILURE 6

#define WINBIO_TYPE_FINGERPRINT 0x00000008

#define WINBIO_SUBTYPE_NO_INFORMATION 0x00
#define WINBIO_SUBTYPE_ANY 0xFF

#define WINBIO_CAPABILITY_SENSOR 0x00000001
#define WINBIO_CAPABILITY_MATCHING 0x00000002
#define WINBIO_CAPABILITY_DATABASE 0x00000004
#define WINBIO_CAPABILITY_PROCESSING 0x00000008
#define WINBIO_CAPABILITY_ENCRYPTION 0x00000010
#define WINBIO_CAPABILITY_NAVIGATION 0x00000020
#define WINBIO_CAPABILITY_INDICATOR 0x00000040

typedef struct _WINBIO_SENSOR_INTERFACE WINBIO_SENSOR_INTERFACE;
typedef struct _WINBIO_ENGINE_INTERFACE WINBIO_ENGINE_INTERFACE;
typedef struct _WINBIO_STORAGE_INTERFACE WINBIO_STORAGE_INTERFACE;

typedef struct _WINBIO_PIPELINE {
    HANDLE SensorHandle;
    HANDLE EngineHandle;
    HANDLE StorageHandle;

    WINBIO_SENSOR_INTERFACE *SensorInterface;
    WINBIO_ENGINE_INTERFACE *EngineInterface;
    WINBIO_STORAGE_INTERFACE *StorageInterface;

    void *SensorContext;
    void *EngineContext;
    void *StorageContext;
} WINBIO_PIPELINE;

typedef struct {
    DWORD MajorVersion, MinorVersion;
} WINBIO_VERSION;

typedef struct {
    USHORT MajorVersion, MinorVersion;
} WINBIO_ADAPTER_INTERFACE_VERSION;

typedef struct {
    ULONG GenericSensorCapabilities;
    ULONG Factor;
    union {
        ULONG Null;
        struct {
            RECT FrameSize;
            POINT FrameOffset;
            ULONG MandatoryOrientation;
            struct {
                WCHAR ColorSensorId[260];
                WCHAR InfraredSensorId[260];
                UINT InfraredSensorRotationAngle;
            } HardwareInfo;
        } FacialFeatures;
        struct {
            ULONG Reserved;
        } Fingerprint;
        struct {
            RECT FrameSize;
            POINT FrameOffset;
            ULONG MandatoryOrientation;
        } Iris;
        struct {
            ULONG Reserved;
        } Voice;
    } Specific;
} WINBIO_EXTENDED_SENSOR_INFO;

typedef struct {
    ULONG GenericEngineCapabilities;
    ULONG Factor;
    union {
        ULONG Null;
        struct {
            ULONG Capabilities;
            struct {
                ULONG Null;
            } EnrollmentRequirements;
        } FacialFeatures;
        struct {
            ULONG Capabilities;
            struct {
                ULONG GeneralSamples;
                ULONG Center;
                ULONG TopEdge;
                ULONG BottomEdge;
                ULONG LeftEdge;
                ULONG RightEdge;
            } EnrollmentRequirements;
        } Fingerprint;
        struct {
            ULONG Capabilities;
            struct {
                ULONG Null;
            } EnrollmentRequirements;
        } Iris;
        struct {
            ULONG Capabilities;
            struct {
                ULONG Null;
            } EnrollmentRequirements;
        } Voice;
    } Specific;
} WINBIO_EXTENDED_ENGINE_INFO;

typedef struct _WINBIO_EXTENDED_STORAGE_INFO { 
    ULONG GenericStorageCapabilities;

    ULONG Factor;
    union {
        ULONG Null;
        struct {
            ULONG Capabilities;
        } FacialFeatures;
        struct {
            ULONG Capabilities;
        } Fingerprint;
        struct {
            ULONG Capabilities;
        } Iris;
        struct {
            ULONG Capabilities;
        } Voice;
    } Specific;
} WINBIO_EXTENDED_STORAGE_INFO;

typedef struct _WINBIO_EXTENDED_ENROLLMENT_STATUS { 
    HRESULT TemplateStatus;
    ULONG RejectDetail;
    ULONG PercentComplete;

    ULONG Factor;
    UCHAR SubFactor;
    union {
        ULONG Null;
        struct {
            RECT BoundingBox;
            LONG Distance;
            struct {
                GUID AdapterId;
                ULONG Data[72];
            } OpaqueEngineData;
        } FacialFeatures;
        struct {
            ULONG GeneralSamples;
            ULONG Center;
            ULONG TopEdge;
            ULONG BottomEdge;
            ULONG LeftEdge;
            ULONG RightEdge;
        } Fingerprint;
        struct {
            RECT EyeBoundingBox_1;
            RECT EyeBoundingBox_2;
            POINT PupilCenter_1;
            POINT PupilCenter_2;
            LONG Distance;
        } Iris;
        struct {
            ULONG Reserved;
        } Voice;
    } Specific;
} WINBIO_EXTENDED_ENROLLMENT_STATUS;

#define WINBIO_NO_PURPOSE_AVAILABLE 0x00
#define WINBIO_PURPOSE_VERIFY 0x01
#define WINBIO_PURPOSE_IDENTIFY 0x02
#define WINBIO_PURPOSE_ENROLL 0x04
#define WINBIO_PURPOSE_ENROLL_FOR_VERIFICATION 0x08
#define WINBIO_PURPOSE_ENROLL_FOR_IDENTIFICATION 0x10
#define WINBIO_PURPOSE_AUDIT 0x80

typedef struct {
    ULONG Size;
    ULONG Offset;
} WINBIO_BIR_DATA;

typedef struct {
    WINBIO_BIR_DATA HeaderBlock;
    WINBIO_BIR_DATA StandardDataBlock;
    WINBIO_BIR_DATA VendorDataBlock;
    WINBIO_BIR_DATA SignatureBlock;
} WINBIO_BIR;

#define WINBIO_ID_TYPE_NULL 0
#define WINBIO_ID_TYPE_WILDCARD 1
#define WINBIO_ID_TYPE_GUID 2
#define WINBIO_ID_TYPE_SID 3

typedef struct _WINBIO_IDENTITY {
    ULONG Type;
    union {
        ULONG Null;
        ULONG Wildcard;
        GUID TemplateGuid;
        struct {
            ULONG Size;
            UCHAR Data[68];
        } AccountSid;
    };
} WINBIO_IDENTITY;

typedef struct {
    USHORT Owner;
    USHORT Type;
} WINBIO_REGISTERED_FORMAT;

typedef union { 
    ULONG Null;

    struct {
        RECT BoundingBox;
        LONG Distance;
        struct {
            GUID AdapterId;
            ULONG Data[72];
        } OpaqueEngineData;
    } FacialFeatures;

    struct {
        RECT EyeBoundingBox_1;
        RECT EyeBoundingBox_2;
        POINT PupilCenter_1;
        POINT PupilCenter_2;
        LONG Distance;
    } Iris;
} WINBIO_PRESENCE_PROPERTIES;

typedef struct {
    ULONG Factor;
    UCHAR SubFactor;
    HRESULT Status;
    ULONG RejectDetail;
    WINBIO_IDENTITY Identity;
    ULONGLONG TrackingId;
    ULONGLONG Ticket;
    WINBIO_PRESENCE_PROPERTIES Properties;
    struct {
        ULONG Size;
        UCHAR Data[32];
    } Authorization;
} WINBIO_PRESENCE;

typedef struct {
    SIZE_T Size;
    UCHAR SubFactor;
} WINBIO_EXTENDED_ENROLLMENT_PARAMETERS;

typedef struct {
    WINBIO_IDENTITY Identity;
    int AntiSpoofBehavior;
} WINBIO_ACCOUNT_POLICY;

typedef struct {
    WINBIO_IDENTITY *Identity;
    UCHAR SubFactor;
    ULONG *IndexVector;
    SIZE_T IndexElementCount;
    UCHAR *TemplateBlob;
    SIZE_T TemplateBlobSize;
    UCHAR *PayloadBlob;
    SIZE_T PayloadBlobSize;
} WINBIO_STORAGE_RECORD;

typedef HRESULT __winfnc IBIO_SENSOR_ATTACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_DETACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_CLEAR_CONTEXT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_QUERY_STATUS_FN(WINBIO_PIPELINE *Pipeline, ULONG *Status);
typedef HRESULT __winfnc IBIO_SENSOR_RESET_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_SET_MODE_FN(WINBIO_PIPELINE *Pipeline, ULONG Mode);
typedef HRESULT __winfnc IBIO_SENSOR_SET_INDICATOR_STATUS_FN(WINBIO_PIPELINE *Pipeline, DWORD IndicatorStatus);
typedef HRESULT __winfnc IBIO_SENSOR_GET_INDICATOR_STATUS_FN(WINBIO_PIPELINE *Pipeline, DWORD *IndicatorStatus);
typedef HRESULT __winfnc IBIO_SENSOR_START_CAPTURE_FN(WINBIO_PIPELINE *Pipeline, UCHAR Purpose, OVERLAPPED **Overlapped);
typedef HRESULT __winfnc IBIO_SENSOR_FINISH_CAPTURE_FN(WINBIO_PIPELINE *Pipeline, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_SENSOR_EXPORT_SENSOR_DATA_FN(WINBIO_PIPELINE *Pipeline, WINBIO_BIR **SampleBuffer, SIZE_T *SampleSize);
typedef HRESULT __winfnc IBIO_SENSOR_CANCEL_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_PUSH_DATA_TO_ENGINE_FN(WINBIO_PIPELINE *Pipeline, UCHAR Purpose, UCHAR Flags, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_SENSOR_CONTROL_UNIT_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_SENSOR_CONTROL_UNIT_PRIVILEGED_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_SENSOR_NOTIFY_POWER_CHANGE_FN(WINBIO_PIPELINE *Pipeline, ULONG PowerEventType);
typedef HRESULT __winfnc IBIO_SENSOR_PIPELINE_INIT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_PIPELINE_CLEANUP_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_ACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_DEACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_SENSOR_QUERY_EXTENDED_INFO_FN(WINBIO_PIPELINE *Pipeline, WINBIO_EXTENDED_SENSOR_INFO *SensorInfo,SIZE_T SensorInfoSize);
typedef HRESULT __winfnc IBIO_SENSOR_QUERY_CALIBRATION_FORMATS_FN(WINBIO_PIPELINE *Pipeline, GUID *FormatArray, SIZE_T FormatArraySize, SIZE_T *FormatCount);
typedef HRESULT __winfnc IBIO_SENSOR_SET_CALIBRATION_FORMAT_FN(WINBIO_PIPELINE *Pipeline, GUID *Format);
typedef HRESULT __winfnc IBIO_SENSOR_ACCEPT_CALIBRATION_DATA_FN(WINBIO_PIPELINE *Pipeline, UCHAR *CalibrationBuffer, SIZE_T CalibrationBufferSize);
typedef HRESULT __winfnc IBIO_SENSOR_ASYNC_IMPORT_RAW_BUFFER_FN(WINBIO_PIPELINE *Pipeline, UCHAR *RawBufferAddress, SIZE_T RawBufferSize, UCHAR **ResultBufferAddress, SIZE_T *ResultBufferSize);
typedef HRESULT __winfnc IBIO_SENSOR_ASYNC_IMPORT_SECURE_BUFFER_FN(WINBIO_PIPELINE *Pipeline, GUID SecureBufferIdentifier, UCHAR *MetadataBufferAddress, SIZE_T MetadataBufferSize, UCHAR **ResultBufferAddress, SIZE_T *ResultBufferSize);
typedef HRESULT __winfnc IBIO_SENSOR_QUERY_PRIVATE_SENSOR_TYPE_FN(WINBIO_PIPELINE *Pipeline, UCHAR *TypeInfoBufferAddress, SIZE_T TypeInfoBufferSize, SIZE_T *TypeInfoDataSize);

typedef struct _WINBIO_SENSOR_INTERFACE {
    WINBIO_ADAPTER_INTERFACE_VERSION Version;
    ULONG Type;
    SIZE_T Size;
    GUID AdapterId;
    IBIO_SENSOR_ATTACH_FN *Attach;
    IBIO_SENSOR_DETACH_FN *Detach;
    IBIO_SENSOR_CLEAR_CONTEXT_FN *ClearContext;
    IBIO_SENSOR_QUERY_STATUS_FN *QueryStatus;
    IBIO_SENSOR_RESET_FN *Reset;
    IBIO_SENSOR_SET_MODE_FN *SetMode;
    IBIO_SENSOR_SET_INDICATOR_STATUS_FN *SetIndicatorStatus;
    IBIO_SENSOR_GET_INDICATOR_STATUS_FN *GetIndicatorStatus;
    IBIO_SENSOR_START_CAPTURE_FN *StartCapture;
    IBIO_SENSOR_FINISH_CAPTURE_FN *FinishCapture;
    IBIO_SENSOR_EXPORT_SENSOR_DATA_FN *ExportSensorData;
    IBIO_SENSOR_CANCEL_FN *Cancel;
    IBIO_SENSOR_PUSH_DATA_TO_ENGINE_FN *PushDataToEngine;
    IBIO_SENSOR_CONTROL_UNIT_FN *ControlUnit;
    IBIO_SENSOR_CONTROL_UNIT_PRIVILEGED_FN *ControlUnitPrivileged;
    IBIO_SENSOR_NOTIFY_POWER_CHANGE_FN *NotifyPowerChange;
    IBIO_SENSOR_PIPELINE_INIT_FN *PipelineInit;
    IBIO_SENSOR_PIPELINE_CLEANUP_FN *PipelineCleanup;
    IBIO_SENSOR_ACTIVATE_FN *Activate;
    IBIO_SENSOR_DEACTIVATE_FN *Deactivate;
    IBIO_SENSOR_QUERY_EXTENDED_INFO_FN *QueryExtendedInfo;
    IBIO_SENSOR_QUERY_CALIBRATION_FORMATS_FN *QueryCalibrationFormats;
    IBIO_SENSOR_SET_CALIBRATION_FORMAT_FN *SetCalibrationFormat;
    IBIO_SENSOR_ACCEPT_CALIBRATION_DATA_FN *AcceptCalibrationData;
    IBIO_SENSOR_ASYNC_IMPORT_RAW_BUFFER_FN *AsyncImportRawBuffer;
    IBIO_SENSOR_ASYNC_IMPORT_SECURE_BUFFER_FN *AsyncImportSecureBuffer;
    IBIO_SENSOR_QUERY_PRIVATE_SENSOR_TYPE_FN *QueryPrivateSensorType;
} WINBIO_SENSOR_INTERFACE;

typedef HRESULT __winfnc IBIO_ENGINE_ATTACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_DETACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_CLEAR_CONTEXT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_PREFERRED_FORMAT_FN(WINBIO_PIPELINE *Pipeline, WINBIO_REGISTERED_FORMAT *StandardFormat, GUID *VendorFormat);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_INDEX_VECTOR_SIZE_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *IndexElementCount);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_HASH_ALGORITHMS_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *AlgorithmCount, SIZE_T *AlgorithmBufferSize, UCHAR *AlgorithmBuffer);
typedef HRESULT __winfnc IBIO_ENGINE_SET_HASH_ALGORITHM_FN(WINBIO_PIPELINE *Pipeline, SIZE_T AlgorithmBufferSize, UCHAR *AlgorithmBuffer);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_SAMPLE_HINT_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *SampleHint);
typedef HRESULT __winfnc IBIO_ENGINE_ACCEPT_SAMPLE_DATA_FN(WINBIO_PIPELINE *Pipeline, WINBIO_BIR *SampleBuffer, SIZE_T SampleSize, UCHAR Purpose, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_ENGINE_EXPORT_ENGINE_DATA_FN(WINBIO_PIPELINE *Pipeline, UCHAR Flags, WINBIO_BIR **SampleBuffer, SIZE_T *SampleSize);
typedef HRESULT __winfnc IBIO_ENGINE_VERIFY_FEATURE_SET_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR SubFactor, BOOLEAN *Match, UCHAR **PayloadBlob, SIZE_T *PayloadBlobSize, UCHAR **HashValue, SIZE_T *HashSize, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_ENGINE_IDENTIFY_FEATURE_SET_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR *SubFactor, UCHAR **PayloadBlob, SIZE_T *PayloadBlobSize, UCHAR **HashValue, SIZE_T *HashSize, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_ENGINE_CREATE_ENROLLMENT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_UPDATE_ENROLLMENT_FN(WINBIO_PIPELINE *Pipeline, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_ENGINE_GET_ENROLLMENT_STATUS_FN(WINBIO_PIPELINE *Pipeline, ULONG *RejectDetail);
typedef HRESULT __winfnc IBIO_ENGINE_GET_ENROLLMENT_HASH_FN(WINBIO_PIPELINE *Pipeline, UCHAR **HashValue, SIZE_T *HashSize);
typedef HRESULT __winfnc IBIO_ENGINE_CHECK_FOR_DUPLICATE_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR *SubFactor, BOOLEAN *Duplicate);
typedef HRESULT __winfnc IBIO_ENGINE_COMMIT_ENROLLMENT_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR SubFactor, UCHAR *PayloadBlob, SIZE_T PayloadBlobSize);
typedef HRESULT __winfnc IBIO_ENGINE_DISCARD_ENROLLMENT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_CONTROL_UNIT_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_ENGINE_CONTROL_UNIT_PRIVILEGED_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_ENGINE_NOTIFY_POWER_CHANGE_FN(WINBIO_PIPELINE *Pipeline, ULONG PowerEventType);
typedef HRESULT __winfnc IBIO_ENGINE_RESERVED_1_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity);
typedef HRESULT __winfnc IBIO_ENGINE_PIPELINE_INIT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_PIPELINE_CLEANUP_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_ACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_DEACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_EXTENDED_INFO_FN(WINBIO_PIPELINE *Pipeline, WINBIO_EXTENDED_ENGINE_INFO *EngineInfo, SIZE_T EngineInfoSize);
typedef HRESULT __winfnc IBIO_ENGINE_IDENTIFY_ALL_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *PresenceCount, WINBIO_PRESENCE **PresenceArray);
typedef HRESULT __winfnc IBIO_ENGINE_SET_ENROLLMENT_SELECTOR_FN(WINBIO_PIPELINE *Pipeline, ULONGLONG SelectorValue);
typedef HRESULT __winfnc IBIO_ENGINE_SET_ENROLLMENT_PARAMETERS_FN(WINBIO_PIPELINE *Pipeline, WINBIO_EXTENDED_ENROLLMENT_PARAMETERS *Parameters);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_EXTENDED_ENROLLMENT_STATUS_FN(WINBIO_PIPELINE *Pipeline, WINBIO_EXTENDED_ENROLLMENT_STATUS *EnrollmentStatus, SIZE_T EnrollmentStatusSize);
typedef HRESULT __winfnc IBIO_ENGINE_REFRESH_CACHE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_ENGINE_SELECT_CALIBRATION_FORMAT_FN(WINBIO_PIPELINE *Pipeline, GUID *FormatArray, SIZE_T FormatCount, GUID *SelectedFormat, SIZE_T *MaxBufferSize);
typedef HRESULT __winfnc IBIO_ENGINE_QUERY_CALIBRATION_DATA_FN(WINBIO_PIPELINE *Pipeline, BOOLEAN *DiscardAndRepeatCapture, UCHAR *CalibrationBuffer, SIZE_T *CalibrationBufferSize, SIZE_T MaxBufferSize);
typedef HRESULT __winfnc IBIO_ENGINE_SET_ACCOUNT_POLICY_FN(WINBIO_PIPELINE *Pipeline, WINBIO_ACCOUNT_POLICY *PolicyItemArray, SIZE_T olicyItemCount);
typedef HRESULT __winfnc IBIO_ENGINE_CREATE_KEY_FN(WINBIO_PIPELINE *Pipeline, const UCHAR* Key, SIZE_T KeySize, UCHAR *KeyIdentifier, SIZE_T KeyIdentifierSize, SIZE_T *ResultSize);
typedef HRESULT __winfnc IBIO_ENGINE_IDENTIFY_FEATURE_SET_SECURE_FN(WINBIO_PIPELINE *Pipeline, const UCHAR* Nonce, SIZE_T NonceSize, const UCHAR* KeyIdentifier, SIZE_T KeyIdentifierSize, WINBIO_IDENTITY *Identity, UCHAR *SubFactor, ULONG *RejectDetail, UCHAR **Authorization, SIZE_T *AuthorizationSize);
typedef HRESULT __winfnc IBIO_ENGINE_ACCEPT_PRIVATE_SENSOR_TYPE_INFO_FN(WINBIO_PIPELINE *Pipeline, const UCHAR* TypeInfoBufferAddress, SIZE_T TypeInfoBufferSize);

typedef struct _WINBIO_ENGINE_INTERFACE {
    WINBIO_ADAPTER_INTERFACE_VERSION Version;
    ULONG Type;
    SIZE_T Size;
    GUID AdapterId;
    IBIO_ENGINE_ATTACH_FN *Attach;
    IBIO_ENGINE_DETACH_FN *Detach;
    IBIO_ENGINE_CLEAR_CONTEXT_FN *ClearContext;
    IBIO_ENGINE_QUERY_PREFERRED_FORMAT_FN *QueryPreferredFormat;
    IBIO_ENGINE_QUERY_INDEX_VECTOR_SIZE_FN *QueryIndexVectorSize;
    IBIO_ENGINE_QUERY_HASH_ALGORITHMS_FN *QueryHashAlgorithms;
    IBIO_ENGINE_SET_HASH_ALGORITHM_FN *SetHashAlgorithm;
    IBIO_ENGINE_QUERY_SAMPLE_HINT_FN *QuerySampleHint;
    IBIO_ENGINE_ACCEPT_SAMPLE_DATA_FN *AcceptSampleData;
    IBIO_ENGINE_EXPORT_ENGINE_DATA_FN *ExportEngineData;
    IBIO_ENGINE_VERIFY_FEATURE_SET_FN *VerifyFeatureSet;
    IBIO_ENGINE_IDENTIFY_FEATURE_SET_FN *IdentifyFeatureSet;
    IBIO_ENGINE_CREATE_ENROLLMENT_FN *CreateEnrollment;
    IBIO_ENGINE_UPDATE_ENROLLMENT_FN *UpdateEnrollment;
    IBIO_ENGINE_GET_ENROLLMENT_STATUS_FN *GetEnrollmentStatus;
    IBIO_ENGINE_GET_ENROLLMENT_HASH_FN *GetEnrollmentHash;
    IBIO_ENGINE_CHECK_FOR_DUPLICATE_FN *CheckForDuplicate;
    IBIO_ENGINE_COMMIT_ENROLLMENT_FN *CommitEnrollment;
    IBIO_ENGINE_DISCARD_ENROLLMENT_FN *DiscardEnrollment;
    IBIO_ENGINE_CONTROL_UNIT_FN *ControlUnit;
    IBIO_ENGINE_CONTROL_UNIT_PRIVILEGED_FN *ControlUnitPrivileged;
    IBIO_ENGINE_NOTIFY_POWER_CHANGE_FN *NotifyPowerChange;
    IBIO_ENGINE_RESERVED_1_FN *Reserved_1;
    IBIO_ENGINE_PIPELINE_INIT_FN *PipelineInit;
    IBIO_ENGINE_PIPELINE_CLEANUP_FN *PipelineCleanup;
    IBIO_ENGINE_ACTIVATE_FN *Activate;
    IBIO_ENGINE_DEACTIVATE_FN *Deactivate;
    IBIO_ENGINE_QUERY_EXTENDED_INFO_FN *QueryExtendedInfo;
    IBIO_ENGINE_IDENTIFY_ALL_FN *IdentifyAll;
    IBIO_ENGINE_SET_ENROLLMENT_SELECTOR_FN *SetEnrollmentSelector;
    IBIO_ENGINE_SET_ENROLLMENT_PARAMETERS_FN *SetEnrollmentParameters;
    IBIO_ENGINE_QUERY_EXTENDED_ENROLLMENT_STATUS_FN *QueryExtendedEnrollmentStatus;
    IBIO_ENGINE_REFRESH_CACHE_FN *RefreshCache; 
    IBIO_ENGINE_SELECT_CALIBRATION_FORMAT_FN *SelectCalibrationFormat;
    IBIO_ENGINE_QUERY_CALIBRATION_DATA_FN *QueryCalibrationData;
    IBIO_ENGINE_SET_ACCOUNT_POLICY_FN *SetAccountPolicy;
    IBIO_ENGINE_CREATE_KEY_FN *CreateKey;
    IBIO_ENGINE_IDENTIFY_FEATURE_SET_SECURE_FN *IdentifyFeatureSetSecure;
    IBIO_ENGINE_ACCEPT_PRIVATE_SENSOR_TYPE_INFO_FN *AcceptPrivateSensorTypeInfo;
} WINBIO_ENGINE_INTERFACE;

typedef HRESULT __winfnc IBIO_STORAGE_ATTACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_DETACH_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_CLEAR_CONTEXT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_CREATE_DATABASE_FN(WINBIO_PIPELINE *Pipeline, GUID *DatabaseId, ULONG Factor, GUID *Format, const char16_t *FilePath, const char16_t *ConnectString, SIZE_T IndexElementCount, SIZE_T InitialSize);
typedef HRESULT __winfnc IBIO_STORAGE_ERASE_DATABASE_FN(WINBIO_PIPELINE *Pipeline, GUID *DatabaseId, const char16_t *FilePath, const char16_t *ConnectString);
typedef HRESULT __winfnc IBIO_STORAGE_OPEN_DATABASE_FN(WINBIO_PIPELINE *Pipeline, GUID *DatabaseId, const char16_t *FilePath, const char16_t *ConnectString);
typedef HRESULT __winfnc IBIO_STORAGE_CLOSE_DATABASE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_GET_DATA_FORMAT_FN(WINBIO_PIPELINE *Pipeline, GUID *Format, WINBIO_VERSION *Version);
typedef HRESULT __winfnc IBIO_STORAGE_GET_DATABASE_SIZE_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *AvailableRecordCount, SIZE_T *TotalRecordCount);
typedef HRESULT __winfnc IBIO_STORAGE_ADD_RECORD_FN(WINBIO_PIPELINE *Pipeline, WINBIO_STORAGE_RECORD *RecordContents);
typedef HRESULT __winfnc IBIO_STORAGE_DELETE_RECORD_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR SubFactor);
typedef HRESULT __winfnc IBIO_STORAGE_QUERY_BY_SUBJECT_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, UCHAR SubFactor);
typedef HRESULT __winfnc IBIO_STORAGE_QUERY_BY_CONTENT_FN(WINBIO_PIPELINE *Pipeline, UCHAR SubFactor, ULONG IndexVector[], SIZE_T IndexElementCount);
typedef HRESULT __winfnc IBIO_STORAGE_GET_RECORD_COUNT_FN(WINBIO_PIPELINE *Pipeline, SIZE_T *RecordCount);
typedef HRESULT __winfnc IBIO_STORAGE_FIRST_RECORD_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_NEXT_RECORD_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_GET_CURRENT_RECORD_FN(WINBIO_PIPELINE *Pipeline, WINBIO_STORAGE_RECORD *RecordContents);
typedef HRESULT __winfnc IBIO_STORAGE_CONTROL_UNIT_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_STORAGE_CONTROL_UNIT_PRIVILEGED_FN(WINBIO_PIPELINE *Pipeline, ULONG ControlCode, UCHAR *SendBuffer, SIZE_T SendBufferSize, UCHAR *ReceiveBuffer, SIZE_T ReceiveBufferSize, SIZE_T *ReceiveDataSize, ULONG *OperationStatus);
typedef HRESULT __winfnc IBIO_STORAGE_NOTIFY_POWER_CHANGE_FN(WINBIO_PIPELINE *Pipeline, ULONG PowerEventType);
typedef HRESULT __winfnc IBIO_STORAGE_PIPELINE_INIT_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_PIPELINE_CLEANUP_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_ACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_DEACTIVATE_FN(WINBIO_PIPELINE *Pipeline);
typedef HRESULT __winfnc IBIO_STORAGE_QUERY_EXTENDED_INFO_FN(WINBIO_PIPELINE *Pipeline, WINBIO_EXTENDED_STORAGE_INFO *StorageInfo, SIZE_T StorageInfoSize);
typedef HRESULT __winfnc IBIO_STORAGE_NOTIFY_DATABASE_CHANGE_FN(WINBIO_PIPELINE *Pipeline, BOOLEAN RecordsAdded);
typedef HRESULT __winfnc IBIO_STORAGE_GET_USER_STATE_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity, ULONGLONG *CurrentState, ULONGLONG *NextState);
typedef HRESULT __winfnc IBIO_STORAGE_INCREMENT_USER_STATE_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY *Identity);
typedef HRESULT __winfnc IBIO_STORAGE_UPDATE_RECORD_BEGIN_FN(WINBIO_PIPELINE *Pipeline, WINBIO_IDENTITY Identity, UCHAR SubFactor, WINBIO_STORAGE_RECORD *RecordContents);
typedef HRESULT __winfnc IBIO_STORAGE_UPDATE_RECORD_COMMIT_FN(WINBIO_PIPELINE *Pipeline, WINBIO_STORAGE_RECORD *RecordContents);

typedef struct _WINBIO_STORAGE_INTERFACE {
    WINBIO_ADAPTER_INTERFACE_VERSION Version;
    ULONG Type;
    SIZE_T Size;
    GUID AdapterId;
    IBIO_STORAGE_ATTACH_FN *Attach;
    IBIO_STORAGE_DETACH_FN *Detach;
    IBIO_STORAGE_CLEAR_CONTEXT_FN *ClearContext;
    IBIO_STORAGE_CREATE_DATABASE_FN *CreateDatabase;
    IBIO_STORAGE_ERASE_DATABASE_FN *EraseDatabase;
    IBIO_STORAGE_OPEN_DATABASE_FN *OpenDatabase;
    IBIO_STORAGE_CLOSE_DATABASE_FN *CloseDatabase;
    IBIO_STORAGE_GET_DATA_FORMAT_FN *GetDataFormat;
    IBIO_STORAGE_GET_DATABASE_SIZE_FN *GetDatabaseSize;
    IBIO_STORAGE_ADD_RECORD_FN *AddRecord;
    IBIO_STORAGE_DELETE_RECORD_FN *DeleteRecord;
    IBIO_STORAGE_QUERY_BY_SUBJECT_FN *QueryBySubject;
    IBIO_STORAGE_QUERY_BY_CONTENT_FN *QueryByContent;
    IBIO_STORAGE_GET_RECORD_COUNT_FN *GetRecordCount;
    IBIO_STORAGE_FIRST_RECORD_FN *FirstRecord;
    IBIO_STORAGE_NEXT_RECORD_FN *NextRecord;
    IBIO_STORAGE_GET_CURRENT_RECORD_FN *GetCurrentRecord;
    IBIO_STORAGE_CONTROL_UNIT_FN *ControlUnit;
    IBIO_STORAGE_CONTROL_UNIT_PRIVILEGED_FN *ControlUnitPrivileged;
    IBIO_STORAGE_NOTIFY_POWER_CHANGE_FN *NotifyPowerChange;
    IBIO_STORAGE_PIPELINE_INIT_FN *PipelineInit;
    IBIO_STORAGE_PIPELINE_CLEANUP_FN *PipelineCleanup;
    IBIO_STORAGE_ACTIVATE_FN *Activate;
    IBIO_STORAGE_DEACTIVATE_FN *Deactivate;
    IBIO_STORAGE_QUERY_EXTENDED_INFO_FN *QueryExtendedInfo;
    IBIO_STORAGE_NOTIFY_DATABASE_CHANGE_FN *NotifyDatabaseChange;
    IBIO_STORAGE_GET_USER_STATE_FN *GetUserState;
    IBIO_STORAGE_INCREMENT_USER_STATE_FN *IncrementUserState;
    IBIO_STORAGE_UPDATE_RECORD_BEGIN_FN *UpdateRecordBegin;
    IBIO_STORAGE_UPDATE_RECORD_COMMIT_FN *UpdateRecordCommit;
} WINBIO_STORAGE_INTERFACE;

typedef DWORD __winfnc (*api_WbioQuerySensorInterface)(WINBIO_SENSOR_INTERFACE** interface);
typedef DWORD __winfnc (*api_WbioQueryEngineInterface)(WINBIO_ENGINE_INTERFACE** interface);
typedef DWORD __winfnc (*api_WbioQueryStorageInterface)(WINBIO_STORAGE_INTERFACE** interface);

#endif