from __future__ import annotations

import secrets
import time
import cryptography.hazmat.primitives.hashes as hashes
import cryptography.hazmat.primitives.hmac as hmac
from .data import *
from .record import *

class TlsRandom(TlsData):
    def __init__(self, unix_time : int, rand_bytes : bytes):
        assert len(rand_bytes) == 28

        super().__init__()
        self.unix_time = unix_time
        self.rand_bytes = rand_bytes
    
    def __repr__(self): return "TlsRandom(time=%d rbytes=%s)" % (self.unix_time, self.rand_bytes.hex())

    @staticmethod
    def create():
        return TlsRandom(int(time.time()), secrets.token_bytes(28))

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsRandom(stream.read_int(4), stream.read(28))
    
    def write(self, stream : TlsDataWriteStream):
        stream.write_int(4, self.unix_time)
        stream.write(self.rand_bytes)


class TlsCertificateType(enum.IntEnum): 
    RSA_SIGN = 1
    DSS_SIGN = 2
    RSA_FIXED_DH = 3
    DSS_FIXED_DH = 4
    ECDSA_SIGN = 64
    RSA_FIXED_ECDH = 65
    ECDSA_FIXED_ECDH = 66

#BROKEN The windows driver has two garbage bytes after the length field
class TlsCertificate(TlsData):
    def __init__(self, data : bytes):
        super().__init__()
        self.data = data

    def __repr__(self): return "TlsCertificate(data=%s)" % self.data.hex()

    @staticmethod
    def read(stream : TlsDataReadStream):
        data_len = stream.read_int(3)
        stream.read(2) #Garbage
        return TlsCertificate(stream.read(data_len))

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(3, len(self.data))
        stream.write(bytes(2)) #Garbage
        stream.write(self.data)


class TlsCipherSuiteId(TlsData):
    suites = {}

    def __init__(self, suite_id : int, suite : type = None):
        super().__init__()
        self.suite_id = suite_id
        self.suite = suite

    def __eq__(self, other : TlsCipherSuiteId):
        if not isinstance(other, TlsCipherSuiteId): return False
        return self.suite_id == other.suite_id

    def __hash__(self): return hash(self.suite_id)
    def __repr__(self): return "CipherSuiteID(id=0x%04x name=%s)" % (self.suite_id, "None" if self.suite is None else self.suite.__name__)

    @staticmethod
    def read(stream : TlsDataReadStream):
        suite_id = stream.read_int(2)
        return TlsCipherSuiteId.suites[suite_id] if suite_id in TlsCipherSuiteId.suites else TlsCipherSuiteId(suite_id)

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(2, self.suite_id)

def tls_cipher_suite(suite_id : int):
    def wrapper(suite : type):
        suite.suite_id = TlsCipherSuiteId(suite_id, suite)
        TlsCipherSuiteId.suites[suite_id] = suite.suite_id
        return suite
    return wrapper

class TlsCompressionMethodId(TlsData):
    methods = {}

    def __init__(self, method_id : int, method : type = None):
        super().__init__()
        self.method_id = method_id
        self.method = method

    def __eq__(self, other : TlsCompressionMethodId):
        if not isinstance(other, TlsCompressionMethodId): return False
        return self.method_id == other.method_id

    def __hash__(self): return hash(self.method_id)
    def __repr__(self): return "CompressionMethodID(id=0x%02x name=%s)" % (self.method_id, "None" if self.method is None else self.method.__name__)

    @staticmethod
    def read(stream : TlsDataReadStream):
        method_id = stream.read_int(1)
        return TlsCompressionMethodId.methods[method_id] if method_id in TlsCompressionMethodId.methods else TlsCompressionMethodId(method_id)

    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, self.method_id)

def tls_compression_method(method_id : int):
    def wrapper(method : type):
        method.method_id = TlsCompressionMethodId(method_id, method)
        TlsCompressionMethodId.methods[method_id] = method.method_id
        return method
    return wrapper


@tls_content_type(20)
class TlsChangeCipherSpec(TlsData):
    content_type : TlsContentType = None
    
    def __repr__(self): return "ChangeCipherSpec()"

    @staticmethod
    def read(stream : TlsDataReadStream):
        assert stream.read_int(1) == 1
        return TlsChangeCipherSpec()
    
    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, 1)


def tls_prf(secret : bytes, label : str, seed : bytes, out_size : int, hash_algo : hashes.HashAlgorithm) -> bytes:
    inp = label.encode("ascii") + seed

    out = bytes()
    a = inp
    while len(out) < out_size:
        #Update A
        h = hmac.HMAC(secret, hash_algo)
        h.update(a)
        a = h.finalize()

        #Update output
        h = hmac.HMAC(secret, hash_algo)
        h.update(a)
        h.update(inp)
        out += h.finalize()

    return out[:out_size]