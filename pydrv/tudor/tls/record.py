from __future__ import annotations

import logging
import tudor
from . import data
from .cipher.suite import *
from .compr.method import *

class TlsRecordLayer:
    encryption_algo : TlsEncryptionAlgorithm
    compr_method : TlsCompressionMethod

    def __init__(self, session : "TlsSession"):
        self.session = session

        self.send_closed = False
        self.recv_closed = False

        self.encryption_algo = TlsNullEncryptionAlgorithm()
        self.remote_encryption_algo = TlsNullEncryptionAlgorithm()
        self.compr_method = TlsNullCompressionMethod()

        self.send_buffer = data.TlsDataPipeStream()
        self.content_buffer = data.TlsDataPipeStream()
        self.content_buffer_type = None

        self.application_data = data.TlsDataPipeStream()
    
    def close(self):
        if self.send_closed: return

        #Send close_notify alert
        self.send(data.TlsAlert(data.TlsAlertLevel.WARNING, data.TlsAlertDescription.CLOSE_NOTIFY))
        self.send_closed = True
        
    def change_cipher_spec(self, encr_algo : TlsEncryptionAlgorithm):
        #Send messsage
        self.send(data.TlsChangeCipherSpec())
        self.flush_content_buffer()
        
        #Switch encryption algorithm
        self.encryption_algo = encr_algo

    def receive_data(self, stream : data.TlsDataReadStream) -> data.TlsDataWriteStream:
        assert not self.recv_closed

        while len(stream) > 0:
            #Read TlsCiphertext and convert to plaintext
            ptext = self.cipher_to_plaintext(data.TlsCiphertext.read(stream))
            if ptext.proto_ver != data.TlsProtocolVersion.current: raise data.TlsAlertException(data.TlsAlertDescription.PROTOCOL_VERSION)

            #We don't implement fragmentation, as the windows driver also doesn't

            fstream = data.TlsDataReadStream(ptext.fragment)
            while len(fstream) > 0:
                #Handle fragment content
                if ptext.content_type == data.TlsChangeCipherSpec.content_type:
                    msg = data.TlsChangeCipherSpec.read(fstream)
                    logging.log(tudor.LOG_TLS, "<- %r" % msg)

                    #Switch encryption algorithms
                    self.remote_encryption_algo = self.encryption_algo
                elif ptext.content_type == data.TlsAlert.content_type:
                    alert = data.TlsAlert.read(fstream)
                    logging.log(tudor.LOG_TLS, "<- %r" % alert)

                    #Handle the alert
                    if alert.descr == data.TlsAlertDescription.CLOSE_NOTIFY:
                        if self.send_closed: logging.log(tudor.LOG_TLS, "Remote confirmed session close")
                        else:
                            self.close()
                            raise Exception("Remote closed session unexpectedly")
                        self.recv_closed = True
                        return
                    if alert.level == data.TlsAlertLevel.WARNING:
                        logging.log(tudor.LOG_WARN, "Received TLS warning alert: %r" % alert)
                    else: 
                        self.close()
                        raise data.TlsAlertException(alert.descr, True)
                elif ptext.content_type == data.TlsHandshakeMessage.content_type: 
                    msg = data.TlsHandshakeMessage.read(fstream)
                    logging.log(tudor.LOG_TLS, "<- %r" % msg)

                    #Proxy to handshake protocol
                    self.session.handshake_proto.receive(msg)
                elif ptext.content_type == data.TlsApplicationData.content_type:
                    msg = data.TlsApplicationData.read(fstream)
                    logging.log(tudor.LOG_TLS, "<- %r" % msg)

                    #Add to buffer
                    self.application_data.write(msg.data)
                else: raise data.TlsAlertException(data.TlsAlertDescription.DECODE_ERROR)

    def send(self, content : data.TlsDataInstance):
        assert not self.send_closed
        logging.log(tudor.LOG_TLS, "-> %r" % content)

        #If we write a different message type, flush content buffer
        if self.content_buffer_type != None and self.content_buffer_type != content.data_type.content_type: self.flush_content_buffer()

        #Write content to content buffer
        content.write(self.content_buffer)
        self.content_buffer_type = content.data_type.content_type

        #If we sent a fatal alert, mark connection as send_closed
        if isinstance(content, data.TlsAlert) and content.level == data.TlsAlertLevel.FATAL: self.send_closed = True

    def has_data(self): return len(self.send_buffer) > 0 or len(self.content_buffer) > 0

    def flush_send_buffer(self):
        self.flush_content_buffer()
        return self.send_buffer.reset()

    def flush_content_buffer(self):
        if self.content_buffer_type != None:
            #Create TlsPlaintext and convert to ciphertext
            ctext = self.plain_to_ciphertext(data.TlsPlaintext(self.content_buffer_type, data.TlsProtocolVersion.current, self.content_buffer.reset()))

            #Write the ciphertext to send buffer
            ctext.write(self.send_buffer)

            self.content_buffer_type = None

    def plain_to_ciphertext(self, pt : data.TlsPlaintext): return self.encryption_algo.encrypt(self.compr_method.compress(pt))
    def cipher_to_plaintext(self, ct : data.TlsCiphertext): return self.compr_method.uncompress(self.remote_encryption_algo.decrypt(ct))
