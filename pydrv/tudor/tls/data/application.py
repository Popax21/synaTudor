from __future__ import annotations

from .data import *
from .record import *

@tls_content_type(23)
class TlsApplicationData(TlsData):
    content_type : TlsContentType = None

    def __init__(self, data : bytes):
        super().__init__()
        self.data = data

    def __repr__(self): return "ApplicationData(data=%s)" % self.data.hex()
    
    @staticmethod
    def read(stream : TlsDataReadStream): return TlsApplicationData(stream.read(len(stream)))
    def write(self, stream : TlsDataWriteStream): stream.write(self.data)