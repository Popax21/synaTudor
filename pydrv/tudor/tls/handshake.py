from __future__ import annotations

import enum
import logging
import tudor
import hashlib
from . import data
from .record import *

class TlsHandshakePhase(enum.Enum):
    IDLE = 1
    SENT_HELLO = 2
    SUITE_HANDSHAKE = 3
    SERVER_DONE = 4
    FINISHED = 5

class TlsHandshakeProtocol:
    def __init__(self, session : "TlsSession", record_layer : TlsRecordLayer):
        self.session = session
        self.record_layer = record_layer

        self.phase = TlsHandshakePhase.IDLE
        self.msg_digest = hashlib.sha256()

        self.cipher_suites = []
        self.compression_methods = [] #BROKEN The windows driver doesn't advertise the NULL compression method
        self.extensions = []

        self.neg_cipher_suite : TlsCipherSuite = None
        self.neg_compr_method : TlsCompressionMethod = None

        self.client_random : data.TlsRandom = None
        self.server_random : data.TlsRandom = None
        
    def in_handshake(self) -> bool: return self.phase != TlsHandshakePhase.IDLE and self.phase != TlsHandshakePhase.FINISHED
    def start_handshake(self):
        assert not self.in_handshake()

        logging.log(tudor.LOG_TLS, "Starting TLS handshake...")
        
        #The windows client always sends a 7 byte session id
        ses_id = self.session.session_id if len(self.session.session_id) == 7 else data.TlsSessionId(bytes(7))
        cipher_suites = [type(s).suite_id for s in self.cipher_suites]
        compr_methods = [type(m).method_id for m in self.compression_methods]

        extensions = list(self.extensions)
        for s in self.cipher_suites: extensions += s.advertise_extensions()
        for m in self.compression_methods: extensions += m.advertise_extensions()

        #Remove extension duplicates (as multiple cipher suites/compression methods may advertise the same extension)
        extensions_no_dupl = []
        for ext in extensions:
            for e2 in extensions_no_dupl:
                if ext.data_type.ext_type == e2.data_type.ext_type: break
            else: extensions_no_dupl.append(ext)

        #Create ClientHello
        self.client_random = data.TlsRandom.create()
        client_hello = data.TlsHandshakeClientHello(data.TlsProtocolVersion.current, self.client_random, ses_id, cipher_suites, compr_methods, extensions_no_dupl)

        #Send ClientHello
        self.send(data.TlsHandshakeMessage(client_hello))
        self.phase = TlsHandshakePhase.SENT_HELLO

    def send(self, msg : data.TlsHandshakeMessage):
        #Update hash
        #BROKEN The windows driver only updates it when the message isn't "Finished"
        if msg.msg_type != data.TlsHandshakeFinished.msg_type:
            stream = data.TlsDataWriteStream()
            msg.write(stream)
            self.msg_digest.update(stream.data)

        #Pass to record layer
        self.record_layer.send(msg)

    def receive(self, msg : data.TlsHandshakeMessage):
        assert self.in_handshake()

        #Update hash
        #BROKEN The windows driver only updates it when the message isn't "Finished"
        if msg.msg_type != data.TlsHandshakeFinished.msg_type:
            stream = data.TlsDataWriteStream()
            msg.write(stream)
            self.msg_digest.update(stream.data)

        msg = msg.content

        #Switch over phase
        if isinstance(msg, data.TlsHandshakeServerHello):
            if self.phase != TlsHandshakePhase.SENT_HELLO: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)

            #Store server random
            self.server_random = msg.rand

            #The windows driver does implement (broken) resuming, but it is never used
            self.session.session_id = msg.ses_id

            #Choose cipher suite and compression method
            for s in self.cipher_suites:
                if s.suite_id == msg.cipher_suite: 
                    self.neg_cipher_suite : TlsCipherSuite = s
                    break
            else: raise data.TlsAlertException(data.TlsAlertDescription.HANDSHAKE_FAILURE)

            if len(self.compression_methods):
                for c in self.compression_methods:
                    if c.method_id == msg.compr_method: 
                        self.neg_compr_method : TlsCompressionMethod = c
                        break
                else: raise data.TlsAlertException(data.TlsAlertDescription.HANDSHAKE_FAILURE)
            else:
                #As we don't advertise any compression methods (NOT STANDARD COMPLIANT), just fallback to the null one
                self.neg_compr_method = TlsNullCompressionMethod()

            #At this point, the cipher suite takes over handshake negotiation
            logging.log(tudor.LOG_TLS, "Cipher suite handshake started")
            self.neg_cipher_suite.start_handshake(self, msg)
            self.phase = TlsHandshakePhase.SUITE_HANDSHAKE
        elif isinstance(msg, data.TlsHandshakeServerHelloDone):
            if self.phase != TlsHandshakePhase.SUITE_HANDSHAKE: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)

            #End the suite handshake
            self.neg_cipher_suite.end_handshake()
            logging.log(tudor.LOG_TLS, "Cipher suite handshake ended")

            #Send "Change Cipher Spec" & "Finished" messages
            self.record_layer.change_cipher_spec(self.neg_cipher_suite.create_encryption_algo())
            self.send(data.TlsHandshakeMessage(
                data.TlsHandshakeFinished(data.tls_prf(self.record_layer.encryption_algo.master_secret, "client finished", self.msg_digest.digest(), 12, self.neg_cipher_suite.hash_algo))
            ))

            #Wait for the server's "Finished" message
            self.phase = TlsHandshakePhase.SERVER_DONE
        elif isinstance(msg, data.TlsHandshakeFinished):
            if self.phase != TlsHandshakePhase.SERVER_DONE: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)

            #Server must have sent "Change Cipher Spec"
            if self.record_layer.remote_encryption_algo != self.record_layer.encryption_algo: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)

            #Handle verify data
            ver_data = data.tls_prf(self.record_layer.encryption_algo.master_secret, "server finished", self.msg_digest.digest(), 12, self.neg_cipher_suite.hash_algo)
            if ver_data != msg.verify_data: raise data.TlsAlertException(data.TlsAlertDescription.DECRYPT_ERROR)

            #The handshake is now done
            self.phase = TlsHandshakePhase.FINISHED
        else: 
            if self.phase != TlsHandshakePhase.SUITE_HANDSHAKE: raise data.TlsAlertException(data.TlsAlertDescription.UNEXPECTED_MESSAGE)

            #Proxy the message to the cipher suite
            self.neg_cipher_suite.receive(msg)
