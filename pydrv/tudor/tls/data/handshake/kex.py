from __future__ import annotations

from ..data import *
from .handshake import *

#BROKEN The sensor doesn't send a ServerKeyExchange message

@tls_handshake_message(16)
class TlsHandshakeClientKeyExchange(TlsData):
    def __init__(self, content : bytes):
        super().__init__()
        self.content = content
    
    def __repr__(self): return "ClientKeyExchange(data=%s)" % self.content.hex()

    @staticmethod
    def read(stream : TlsDataReadStream): return TlsHandshakeClientKeyExchange(stream.read(len(stream)))
    def write(self, stream : TlsDataWriteStream): stream.write(self.content)