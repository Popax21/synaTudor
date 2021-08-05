from __future__ import annotations

from .handshake import *

@tls_handshake_message(20)
class TlsHandshakeFinished(TlsData):
    msg_type : TlsHandshakeMessageType = None

    def __init__(self, verify_data : bytes):
        super().__init__()
        self.verify_data = verify_data

    def __repr__(self): return "Finished(verify_data=%s)" % self.verify_data.hex()

    @staticmethod
    def read(stream : TlsDataReadStream): return TlsHandshakeFinished(stream.read(len(stream)))
    def write(self, stream : TlsDataWriteStream): stream.write(self.verify_data)