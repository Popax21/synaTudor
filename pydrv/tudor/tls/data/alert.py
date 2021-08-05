from __future__ import annotations

import enum

class TlsAlertLevel(enum.IntEnum):
    WARNING = 1
    FATAL = 2

class TlsAlertDescription(enum.IntEnum):
    CLOSE_NOTIFY = 0
    UNEXPECTED_MESSAGE = 10
    BAD_RECORD_MAC = 20
    DECRYPTION_FAILED_RESERVED = 21
    RECORD_OVERFLOW = 22
    DECOMPRESSION_FAILURE = 30
    HANDSHAKE_FAILURE = 40
    NO_CERTIFICATE_RESERVED = 41
    BAD_CERTIFICATE = 42
    UNSUPPORTED_CERTIFICATE = 43
    CERTIFICATE_REVOKED = 44
    CERTIFICATE_EXPIRED = 45
    CERTIFICATE_UNKNOWN = 46
    ILLEGAL_PARAMETER = 47
    UNKNOWN_CA = 48
    ACCESS_DENIED = 49
    DECODE_ERROR = 50
    DECRYPT_ERROR = 51
    EXPORT_RESTRICTION_RESERVED = 60
    PROTOCOL_VERSION = 70
    INSUFFICIENT_SECURITY = 71
    INTERNAL_ERROR = 80
    USER_CANCELED = 90
    NO_RENEGOTIATION = 100
    UNSUPPORTED_EXTENSION = 110

class TlsAlertException(Exception):
    def __init__(self, descr : TlsAlertDescription, remote : bool = False):
        self.descr = descr
        self.remote = remote

    def __str__(self): return repr(self)
    def __repr__(self): return "TlsAlertException(descr=%r remote=%r)" % (self.descr, self.remote)
