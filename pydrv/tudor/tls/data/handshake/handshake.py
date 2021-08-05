from __future__ import annotations

from ..data import *
from ..crypto import *
from ..record import *

class TlsHandshakeMessageType:
    msg_types = {}

    def __init__(self, msg_id : int, content_type : TlsDataType):
        self.msg_id = msg_id
        self.content_type = content_type

    def __eq__(self, other : TlsHandshakeMessageType):
        if not isinstance(other, TlsHandshakeMessageType): return False
        return self.msg_id == other.msg_id
    def __hash__(self): return hash(self.msg_id)
    def __repr__(self): return "0x%02x" % self.msg_id

    def read(self, stream : TlsDataReadStream): 
        msg = TlsHandshakeMessage.read(stream)
        assert msg.msg_type == self
        return msg

@tls_content_type(22)
class TlsHandshakeMessage(TlsData): 
    content_type : TlsContentType = None

    def __init__(self, content : TlsDataInstance):
        assert isinstance(content.data_type.msg_type, TlsHandshakeMessageType)
        super().__init__()
        self.msg_type = content.data_type.msg_type
        self.content = content
    
    def __repr__(self): return "HandshakeMessage(type=%r content=%r)" % (self.msg_type, self.content)

    @staticmethod
    def read(stream : TlsDataReadStream) -> TlsHandshakeMessage:
        #Read message id & size
        msg_id, msg_size = stream.read_int(1), stream.read_int(3)
        if msg_id not in TlsHandshakeMessageType.msg_types: raise TlsAlertException(TlsAlertDescription.ILLEGAL_PARAMETER)
        msg_type : TlsHandshakeMessageType = TlsHandshakeMessageType.msg_types[msg_id]

        #Create content substream & read content
        cstream = stream.create_substream(msg_size)
        return TlsHandshakeMessage(msg_type.content_type.read(cstream))

    def write(self, stream : TlsDataWriteStream):
        #Write message id
        stream.write_int(1, self.msg_type.msg_id)

        #Write content to content substream
        cstream = TlsDataWriteStream()
        self.content.write(cstream)

        #Write message size & content data
        stream.write_int(3, len(cstream))
        stream.write(cstream.data)

    @classmethod
    def convert(content : TlsDataInstance): return TlsHandshakeMessage(content)

def tls_handshake_message(msg_id : int):
    def wrapper(content_type : TlsDataType):
        content_type.msg_type = TlsHandshakeMessageType(msg_id, content_type)
        TlsHandshakeMessageType.msg_types[msg_id] = content_type.msg_type
        return content_type
    return wrapper