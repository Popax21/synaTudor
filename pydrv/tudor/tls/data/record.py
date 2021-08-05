from __future__ import annotations

from .data import *
from .alert import *

class TlsProtocolVersion(TlsData):
    current : TlsProtocolVersion = None

    def __init__(self, major : int, minor : int):
        self.major = major
        self.minor = minor

    def __eq__(self, other : TlsProtocolVersion):
        if not isinstance(other, TlsProtocolVersion): return False
        return self.major == other.major and self.minor == other.minor

    def __str__(self): return "%d.%d" % (self.major, self.minor)
    def __repr__(self): return "ProtocolVersion(%d.%d)" % (self.major, self.minor)

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsProtocolVersion(stream.read_int(1), stream.read_int(1))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.major)
        stream.write_int(1, self.minor)
TlsProtocolVersion.current = TlsProtocolVersion(3, 3)

class TlsContentType:
    types = {}

    def __init__(self, type_id : int, data_type : TlsDataType):
        self.type_id = type_id
        self.data_type = data_type

    def __eq__(self, other : TlsContentType):
        if not isinstance(other, TlsContentType): return False
        return self.type_id == other.type_id
    def __hash__(self): return hash(self.type_id)
    def __repr__(self): return "ContentType(id=0x%04x name=%s)" % (self.type_id, self.data_type)

    def read(self, stream : TlsDataReadStream):
        ptext = TlsPlaintext.read(stream)
        assert ptext.content_type == self
        return ptext

def tls_content_type(ct_id : int):
    def wrapper(ctype : type):
        ctype.content_type = TlsContentType(ct_id, ctype)
        TlsContentType.types[ct_id] = ctype.content_type
        return ctype
    return wrapper

#BROKEN The windows driver doesn't enforce any size limits
class TlsPlaintext(TlsData):
    def __init__(self, content_type : TlsContentType, proto_ver : TlsProtocolVersion, fragment : bytes):
        super().__init__()
        self.content_type = content_type
        self.proto_ver = proto_ver
        self.fragment = fragment

    @staticmethod
    def read(stream : TlsDataReadStream):
        ct_type_id, proto_ver, frag_len = stream.read_int(1), TlsProtocolVersion.read(stream), stream.read_int(2)
        return TlsCompressed(TlsContentType.types[ct_type_id], proto_ver, stream.read(frag_len))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.content_type.type_id)
        self.proto_ver.write(stream)
        stream.write_int(2, len(self.fragment))
        stream.write(self.fragment)

class TlsCompressed(TlsData):
    def __init__(self, content_type : TlsContentType, proto_ver : TlsProtocolVersion, fragment : bytes):
        super().__init__()
        self.content_type = content_type
        self.proto_ver = proto_ver
        self.fragment = fragment

    @staticmethod
    def read(stream : TlsDataReadStream):
        ct_type_id, proto_ver, frag_len = stream.read_int(1), TlsProtocolVersion.read(stream), stream.read_int(2)
        return TlsCompressed(TlsContentType.types[ct_type_id], proto_ver, stream.read(frag_len))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.content_type.type_id)
        self.proto_ver.write(stream)
        stream.write_int(2, len(self.fragment))
        stream.write(self.fragment)

class TlsCiphertext(TlsData):
    def __init__(self, content_type : TlsContentType, proto_ver : TlsProtocolVersion, fragment : bytes):
        super().__init__()
        self.content_type = content_type
        self.proto_ver = proto_ver
        self.fragment = fragment

    @staticmethod
    def read(stream : TlsDataReadStream):
        ct_type_id, proto_ver, frag_len = stream.read_int(1), TlsProtocolVersion.read(stream), stream.read_int(2)
        return TlsCompressed(TlsContentType.types[ct_type_id], proto_ver, stream.read(frag_len))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.content_type.type_id)
        self.proto_ver.write(stream)
        stream.write_int(2, len(self.fragment))
        stream.write(self.fragment)


@tls_content_type(21)
class TlsAlert(TlsData):
    content_type : TlsContentType = None

    def __init__(self, level : TlsAlertLevel, descr : TlsAlertDescription):
        super().__init__()
        self.level = level
        self.descr = descr
    
    def __repr__(self): return "TlsAlert(level=%r descr=%r)" % (self.level, self.descr)

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsAlert(TlsAlertLevel(stream.read_int(1)), TlsAlertDescription(stream.read_int(1)))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.level)
        stream.write_int(1, self.descr)