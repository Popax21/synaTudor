from __future__ import annotations

import enum
from ..data import *
from .handshake import *

#BROKEN The windows driver calculates the length field wrong
#BROKEN The standard allows a list of certificates, not only a single one
@tls_handshake_message(11)
class TlsHandshakeCertificate(TlsData):
    msg_type : TlsHandshakeMessageType = None

    cert_vec = TlsVectorType(TlsCertificate, min_size=0, max_size=0xffffff)

    def __init__(self, cert : TlsCertificate):
        super().__init__()
        self.cert = cert

    def __repr__(self): return "Certificate(cert=%r)" % self.cert

    @staticmethod
    def read(stream : TlsDataReadStream):
        cert_len = stream.read_int(3)
        cert = TlsCertificate.read(stream)
        assert len(cert.data) == cert_len
        return TlsHandshakeCertificate(cert)
    
    def write(self, stream : TlsDataWriteStream):
        stream.write_int(3, len(self.cert.data))
        self.cert.write(stream)

#BROKEN The windows driver has a completely wrong structure
@tls_handshake_message(13)
class TlsHandshakeCertificateRequest(TlsData):
    msg_type : TlsHandshakeMessageType = None

    cert_type_vec = TlsVectorType(TlsIntegerType(1), min_size=1, max_size=0xff)

    def __init__(self, cert_types : list):
        super().__init__()
        self.cert_types = cert_types

    def __repr__(self): return "CertificateRequest(types=%r)" % self.cert_types

    @staticmethod
    def read(stream : TlsDataReadStream):
        certs = TlsHandshakeCertificateRequest.cert_type_vec.read(stream).elements
        stream.read(2) #Some garbage bytes
        return TlsHandshakeCertificateRequest(certs)

    def write(self, stream : TlsDataWriteStream):
        TlsVector(TlsHandshakeCertificateRequest.cert_type_vec, self.cert_types).write(stream)
        stream.write(bytes(2)) #Some garbage bytes

@tls_handshake_message(15)
class TlsHandshakeCertificateVerify(TlsData):
    msg_type : TlsHandshakeMessageType = None

    def __init__(self, signed_hash : bytes):
        super().__init__()
        self.signed_hash = signed_hash
    
    def __repr__(self): return "CertificateVerify(signed_hash=%s)" % self.signed_hash.hex()

    @staticmethod
    def read(stream : TlsDataReadStream): return TlsHandshakeCertificateVerify(stream.read(len(stream)))
    def write(self, stream : TlsDataWriteStream): stream.write(self.signed_hash)