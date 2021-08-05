from __future__ import annotations

import cryptography.hazmat.primitives.hashes as hashes
from .. import data
from .encr import *

class TlsCipherSuite:
    suite_id : data.TlsCipherSuiteId = None
    hash_algo : hashes.HashAlgorithm

    def advertise_extensions(self) -> list: raise NotImplementedError()

    def start_handshake(self, handshake_proto : "TlsHandshakeProtocol", server_hello : data.TlsHandshakeServerHello): raise NotImplementedError()
    def receive(self, msg : data.TlsDataInstance): raise NotImplementedError()
    def end_handshake(self): raise NotImplementedError()

    def create_encryption_algo(self) -> TlsEncryptionAlgorithm: raise NotImplementedError()

class TlsNullEncryptionAlgorithm(TlsEncryptionAlgorithm):
    def __init__(self): pass
    def encrypt(self, compr : data.TlsCompressed) -> data.TlsCiphertext: return data.TlsCiphertext(compr.content_type, compr.proto_ver, compr.fragment)
    def decrypt(self, ctext : data.TlsCiphertext) -> data.TlsCompressed: return data.TlsCompressed(ctext.content_type, ctext.proto_ver, ctext.fragment)