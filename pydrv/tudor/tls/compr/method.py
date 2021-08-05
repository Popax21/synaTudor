from __future__ import annotations

from .. import data

class TlsCompressionMethod:
    method_id : data.TlsCompressionMethodId = None
    def advertise_extensions(self) -> list: raise NotImplementedError()
    def compress(self, ptext : data.TlsPlaintext) -> data.TlsCompressed: raise NotImplementedError()
    def uncompress(self, compr : data.TlsCompressed) -> data.TlsPlaintext: raise NotImplementedError()

@data.tls_compression_method(0)
class TlsNullCompressionMethod(TlsCompressionMethod):
    def advertise_extensions(self) -> list: return []
    
    def compress(self, ptext : data.TlsPlaintext) -> data.TlsCompressed:
        return data.TlsCompressed(ptext.content_type, ptext.proto_ver, ptext.fragment)

    def uncompress(self, compr : data.TlsCompressed) -> data.TlsPlaintext:
        return data.TlsPlaintext(compr.content_type, compr.proto_ver, compr.fragment)