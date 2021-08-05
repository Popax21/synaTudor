from __future__ import annotations

import time
import secrets
import logging
import struct
import typing
import tudor

from . import data
from .record import *
from .handshake import *
from .cipher import *

def _critical_method(mthd : callable):
    def wrapper(self, *args, **kwargs):
        oerr : Exception
        alert : data.TlsAlert
        try: return mthd(self, *args, **kwargs)
        except data.TlsAlertException as e: 
            if e.remote: raise e
            oerr = e
            alert = data.TlsAlert(data.TlsAlertLevel.FATAL, e.descr)
        except Exception as e: 
            oerr = e
            alert = data.TlsAlert(data.TlsAlertLevel.FATAL, data.TlsAlertDescription.INTERNAL_ERROR)

        if self.record_layer != None and not self.record_layer.send_closed:
            try:
                self.record_layer.flush_send_buffer()
                self.record_layer.send(alert)
                
                if self.established: self.record_layer.receive_data(self.comm.send_command(self.record_layer.flush_send_buffer(), 0x964, raw=True))
                else: self.record_layer.receive_data(self.send_tls_data_command(self.record_layer.flush_send_buffer()))
            except Exception: logging.exception("Error while sending fatal alert")

        raise oerr
    return wrapper

class TlsSession:
    def __init__(self, comm : tudor.CommunicationInterface, remote_key : TlsRemoteKey):
        self.comm = comm
        self.established = False
        self.session_id = data.TlsSessionId.none
        self.remote_key = remote_key

    @_critical_method
    def establish(self):
        logging.log(tudor.LOG_TLS, "Establishing TLS session...")

        #Get remote established state
        remote_established = self.comm.remote_tls_status()

        #Handle possible combinations 
        if self.established and remote_established: logging.log(tudor.LOG_TLS, "    host and remote already are in a session")
        if self.established and not remote_established: raise Exception("Host is in a session, but device isn't!")
        if not self.established and remote_established: raise Exception("Device is in a session, but host isn't!")
        
        #Initialize layers
        self.record_layer = TlsRecordLayer(self)
        self.handshake_proto = TlsHandshakeProtocol(self, self.record_layer)
        self.handshake_proto.cipher_suites += self.remote_key.cipher_suites

        #Do handshake
        self.handshake_proto.start_handshake()
        while self.record_layer.has_data():
            self.record_layer.receive_data(self.send_tls_data_command(self.record_layer.flush_send_buffer()))
        assert self.handshake_proto.phase == TlsHandshakePhase.FINISHED

        self.established = True
        assert self.comm.remote_tls_status()

    @_critical_method
    def close(self):
        assert self.established

        #Close the session
        logging.log(tudor.LOG_TLS, "Closing TLS session...")
        self.record_layer.close()
        while self.record_layer.has_data():
            self.record_layer.receive_data(data.TlsDataReadStream(self.comm.send_command(self.record_layer.flush_send_buffer(), 0x964, raw=True)))

        #Reset layers
        self.record_layer = None
        self.handshake_proto = None

        self.established = False

    @_critical_method
    def wrap(self, pdata : bytes) -> bytes:
        if not self.established: return pdata

        #Send application data message
        self.record_layer.send(data.TlsApplicationData(pdata))

        return self.record_layer.flush_send_buffer()

    @_critical_method
    def unwrap(self, cdata : bytes) -> bytes:
        if not self.established: return cdata

        #"Receive" data
        self.record_layer.receive_data(data.TlsDataReadStream(cdata))

        return self.record_layer.application_data.reset()

    def send_tls_data_command(self, tdata : bytes):
        return data.TlsDataReadStream(self.comm.send_command(struct.pack("<Bxxx", tudor.Command.TLS_DATA) + tdata, 0x100, raw=True))