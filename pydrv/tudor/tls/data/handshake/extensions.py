from __future__ import annotations

import enum
from .handshake import *

class TlsHandshakeExtensionType:
    extensions = {}

    def __init__(self, ext_id : int, data_type : TlsDataType):
        self.ext_id = ext_id
        self.ext_data_type = data_type

    def __eq__(self, other : TlsHandshakeExtensionType):
        if not isinstance(other, TlsHandshakeExtensionType): return False
        return self.ext_id == other.ext_id
    def __hash__(self): return hash(self.ext_id)
    def __repr__(self): return "0x%04x" % self.ext_id

    def read(self, stream : TlsDataReadStream):
        ext = TlsHandshakeExtension.read(stream)
        assert ext.ext_type == self
        return ext

class TlsHandshakeExtension(TlsData):
    def __init__(self, ext : TlsDataInstance):
        assert isinstance(ext.data_type.ext_type, TlsHandshakeExtensionType)
        super().__init__()
        self.ext_type = ext.data_type.ext_type
        self.ext_data = ext
    
    def __repr__(self): return "HandshakeExtension(type=%r data=%r)" % (self.ext_type, self.ext_data)

    @staticmethod
    def read(stream : TlsDataReadStream):
        #Read extension id & size
        ext_id, ext_size = stream.read_int(2), stream.read_int(2)
        ext_type : TlsHandshakeExtensionType = TlsHandshakeExtensionType.extensions[ext_id]

        assert ext_size <= 0xfffe

        #Create data substream & read data
        dstream = stream.create_substream(ext_size)
        return TlsHandshakeExtension(ext_type.data_type.read(dstream))

    def write(self, stream : TlsDataWriteStream):
        #Write extension id
        stream.write_int(2, self.ext_type.ext_id)

        #Write extension data to substream
        dstream = TlsDataWriteStream()
        self.ext_data.write(dstream)

        #Write data size & data content
        assert len(stream) <= 0xfffe
        stream.write_int(2, len(dstream))
        stream.write(dstream.data)

    @staticmethod
    def convert(ext : TlsDataInstance):
        return TlsHandshakeExtension(ext)

def tls_handshake_extension(ext_id : int):
    def wrapper(data_type : TlsDataType):
        data_type.ext_type = TlsHandshakeExtensionType(ext_id, data_type)
        TlsHandshakeExtensionType.extensions[ext_id] = data_type.ext_type
        return data_type
    return wrapper


@tls_handshake_extension(10)
class TlsHandshakeExtensionSupportedGroups(TlsData):
    ext_type : TlsHandshakeExtensionType = None

    curves_vtype = TlsVectorType(TlsIntegerType(2), min_size=2, max_size=0xffff)

    def __init__(self, curves : list):
        super().__init__()
        self.curves = curves

    def __repr__(self): return "supported_groups(curves=%r)" % self.curves

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsHandshakeExtensionSupportedGroups(
            TlsVector.read(stream, TlsHandshakeExtensionSupportedGroups.curves_vtype).elements
        )

    def write(self, stream : TlsDataWriteStream):
        TlsVector(TlsHandshakeExtensionSupportedGroups.curves_vtype, self.curves).write(stream)

@tls_handshake_extension(11)
class TlsHandshakeExtensionECPointFormats(TlsData):
    ext_type : TlsHandshakeExtensionType = None

    formats_vtype = TlsVectorType(TlsIntegerType(1), min_size=1, max_size=0xff)

    UNCOMPRESSED = 0

    def __init__(self, formats : list): 
        super().__init__()
        self.formats = formats

    def __repr__(self): return "ec_point_formats(formats=%r)" % self.formats

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsHandshakeExtensionECPointFormats(
            TlsVector.read(stream, TlsHandshakeExtensionECPointFormats.formats_vtype).elements
        )
    
    def write(self, stream : TlsDataWriteStream):
        TlsVector(TlsHandshakeExtensionECPointFormats.formats_vtype, self.formats).write(stream)