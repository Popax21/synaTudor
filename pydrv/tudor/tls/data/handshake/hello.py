from __future__ import annotations

from .handshake import *
from .extensions import *

class TlsSessionId(TlsData):
    none : TlsSessionId = None

    def __init__(self, data : bytes):
        assert 0 <= len(data) and len(data) <= 32
        self.data = data

    def __len__(self): return len(self.data)    
    def __eq__(self, o : TlsSessionId): return isinstance(o, TlsSessionId) and self.data == o.data
    def __ne__(self, o : TlsSessionId): return not (self == o)
    def __str__(self): return "SessionId(len=%d)" % len(self.data) if len(self.data) > 0 else "none"
    def __repr__(self): return "SessionId(data='%s')" % self.data.hex()

    @staticmethod
    def read(stream : TlsDataReadStream):
        return TlsSessionId(stream.read(stream.read_int(1)))
    
    def write(self, stream : TlsDataWriteStream):
        stream.write_int(1, len(self.data))
        stream.write(self.data)
TlsSessionId.none = TlsSessionId(bytes())

@tls_handshake_message(0x0)
class TlsHandshakeHelloRequest(TlsData):
    msg_type : TlsHandshakeMessageType = None
    
    def __repr__(self): return "HelloRequest()"

    @staticmethod
    def read(stream : TlsDataReadStream): return TlsHandshakeHelloRequest()
    def write(self, stream : TlsDataWriteStream): pass

#BROKEN The developers didn't give the "extensions" field a length value!

@tls_handshake_message(0x1)
class TlsHandshakeClientHello(TlsData):
    msg_type : TlsHandshakeMessageType = None

    cipher_suites_vtype = TlsVectorType(TlsCipherSuiteId, min_size=2, max_size=0xfffe)
    compr_methods_vtype = TlsVectorType(TlsCompressionMethodId, min_size=0, max_size=0xff) #BROKEN The windows driver doesn't advertise any compression methods

    def __init__(self, proto_ver : TlsProtocolVersion, rand : TlsRandom, ses_id : TlsSessionId, cipher_suites : list, compr_methods : list, extensions : list):
        super().__init__()
        self.proto_ver = proto_ver
        self.rand = rand
        self.ses_id = ses_id
        self.cipher_suites = cipher_suites
        self.compr_methods = compr_methods
        self.extensions = extensions
    
    def __repr__(self): return "ClientHello(ver=%r rand=%r ses_id=%r cipher_suites=%r compr_methods=%r extensions=%r)" % (self.proto_ver, self.rand, self.ses_id, self.cipher_suites, self.compr_methods, self.extensions)

    @staticmethod
    def read(stream : TlsDataReadStream):
        #Read structure fields
        proto_ver = TlsProtocolVersion.read(stream)
        rand = TlsRandom.read(stream),
        ses_id = TlsSessionId.read(stream),
        cipher_suites = TlsVector.read(stream, TlsHandshakeClientHello.cipher_suites_vtype).elements
        compr_methods = TlsVector.read(stream, TlsHandshakeClientHello.compr_methods_vtype).elements

        #Read extensions
        exts = []
        while len(stream) > 0: exts.append(TlsHandshakeExtension.read(stream).ext_data)

        return TlsHandshakeClientHello(proto_ver, rand, ses_id, cipher_suites, compr_methods, exts)

    def write(self, stream : TlsDataWriteStream):
        self.proto_ver.write(stream)
        self.rand.write(stream)
        self.ses_id.write(stream)
        TlsVector(TlsHandshakeClientHello.cipher_suites_vtype, self.cipher_suites).write(stream)
        TlsVector(TlsHandshakeClientHello.compr_methods_vtype, self.compr_methods).write(stream)
        for e in self.extensions: TlsHandshakeExtension(e).write(stream)

@tls_handshake_message(0x2)
class TlsHandshakeServerHello(TlsData):
    msg_type : TlsHandshakeMessageType = None

    def __init__(self, proto_ver : TlsProtocolVersion, rand : TlsRandom, ses_id : TlsSessionId, cipher_suite : TlsCipherSuiteId, compr_method : TlsCompressionMethodId, extensions : list):
        super().__init__()
        self.proto_ver = proto_ver
        self.rand = rand
        self.ses_id = ses_id
        self.cipher_suite = cipher_suite
        self.compr_method = compr_method
        self.extensions = extensions
    
    def __repr__(self): return "ServerHello(ver=%r rand=%r ses_id=%r cipher_suite=%r compr_method=%r extensions=%r)" % (self.proto_ver, self.rand, self.ses_id, self.cipher_suite, self.compr_method, self.extensions)

    @staticmethod
    def read(stream : TlsDataReadStream):
        #Read structure fields
        proto_ver = TlsProtocolVersion.read(stream)
        rand = TlsRandom.read(stream)
        ses_id = TlsSessionId.read(stream)
        cipher_suite = TlsCipherSuiteId.read(stream)
        compr_method = TlsCompressionMethodId.read(stream)

        #Read extensions
        exts = []
        while len(stream) > 0: exts.append(TlsHandshakeExtension.read(stream).ext_data)

        return TlsHandshakeServerHello(proto_ver, rand, ses_id, cipher_suite, compr_method, exts)

    def write(self, stream : TlsDataWriteStream):
        self.proto_ver.write(stream)
        self.rand.write(stream)
        self.ses_id.write(stream)
        self.cipher_suite.write(stream)
        self.compr_method.write(stream)
        for e in self.extensions: TlsHandshakeExtension(e).write(stream)

@tls_handshake_message(0xe)
class TlsHandshakeServerHelloDone(TlsData):
    msg_type : TlsHandshakeMessageType = None

    def __repr__(self): return "ServerHelloDone()"

    @staticmethod
    def read(stream : TlsDataReadStream): return TlsHandshakeServerHelloDone()
    def write(self, stream : TlsDataWriteStream): pass