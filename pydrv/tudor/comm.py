from __future__ import annotations

import struct
import array
import time
import logging
import threading
import usb.core
import usb.util
import tudor.tls
from .log import *

SUCCESS_STATUS = [0, 0x412, 0x5cc]

class Command:
    GET_VERSION = 0x1
    REST = 0x5
    PEEK = 0x7
    POKE = 0x8
    PROVISION = 0xe
    RESET_OWNERSHIP = 0x10
    GET_START_INFO = 0x19
    LED_EX2 = 0x39
    STORAGE_INFO_GET = 0x3e
    STORAGE_PART_FORMAT = 0x3f
    STORAGE_PART_READ = 0x40
    STORAGE_PART_WRITE = 0x41
    TLS_DATA = 0x44
    DB_OBJECT_CREATE = 0x47
    TAKE_OWNERSHIP_EX2 = 0x4f
    GET_CERTIFICATE_EX = 0x50
    SET_IDLE_TIMEOUT = 0x57
    BOOTLOADER_PATCH = 0x7d
    FRAME_READ = 0x7f
    FRAME_ACQ = 0x80
    FRAME_FINISH = 0x81
    FRAME_STATE_GET = 0x82
    EVENT_CONFIG = 0x86
    EVENT_READ = 0x87
    FRAME_STREAM = 0x8b
    READ_IOTA = 0x8e
    PAIR = 0x93
    DB2_GET_DB_INFO = 0x9e
    DB2_GET_OBJ_LIST = 0x9f
    DB2_GET_OBJ_INFO = 0xa0
    DB2_GET_OBJ_DATA = 0xa1
    DB2_DELETE_OBJ = 0xa3
    DB2_CLEANUP = 0xa3
    DB2_FORMAT = 0xa5

class CommandFailedException(Exception):
    def __init__(self, status : int):
        super().__init__("Command failed: status 0x%04x" % status)
        self.status = status

class CommunicationInterface:
    def close(self): raise NotImplementedError()

    def reset(self): raise NotImplementedError()
    def send_command(self, cmd : bytes, resp_size : int, timeout : int = 2000, raw : bool = False) -> bytes: raise NotImplementedError()

    def set_tls_session(self, session : tudor.tls.TlsSession): raise NotImplementedError()
    def remote_tls_status(self) -> bool: raise NotImplementedError()

    def write_dft(self, data : bytes): raise NotImplementedError()
    def get_event_data(self) -> bytes: raise NotImplementedError()

class USBCommunication(CommunicationInterface):
    def __init__(self, dev):
        self.dev = dev
        self.dev.set_configuration()

        #Detach kernel drivers
        for i in range(self.dev.get_active_configuration().bNumInterfaces):
            if dev.is_kernel_driver_active(i):
                dev.detach_kernel_driver(i)

        #Claim the interface
        usb.util.claim_interface(dev, 0)
        self.intf = self.dev.get_active_configuration()[(0, 0)]

        #Find endpoints
        self.cmd_ep = self.intf[0]
        self.resp_ep = self.intf[1]
        self.intr_ep = usb.util.find_descriptor(self.intf,
            custom_match = lambda e:
                usb.util.endpoint_type(e.bEndpointAddress) == usb.util.ENDPOINT_TYPE_INTR
        )

        assert not self.cmd_ep is None
        assert not self.resp_ep is None
        assert not self.intr_ep is None

        #Init the TLS session
        self.tls_session = None

    def close(self):
        usb.util.release_interface(self.dev, 0)
        self.dev.reset()
        self.dev = None

    def reset(self):
        self.tls_session = None
        self.dev.reset()

    def send_command(self, cmd, resp_size, timeout = 2000, raw = False):
        #Wrap and send command
        wcmd = self.tls_session.wrap(cmd) if self.tls_session != None else cmd
        self.cmd_ep.write(wcmd, timeout)
        
        #Receive wrapped resonse
        if self.tls_session != None: resp_size += 0x45
        buf = array.array('B', [0 for _ in range(resp_size)])
        wresp = bytes(buf[:self.resp_ep.read(buf, timeout)])

        #Unwrap and parse response
        resp = self.tls_session.unwrap(wresp) if self.tls_session != None else wresp

        if not raw:
            if len(resp) < 2: raise Exception("Invalid response")
            if not struct.unpack("<H", resp[:2])[0] in SUCCESS_STATUS: raise CommandFailedException(struct.unpack("<H", resp[:2])[0])

        return resp

    def set_tls_session(self, session : tudor.tls.TlsSession): self.tls_session = session
    def remote_tls_status(self) -> bool:
        return struct.unpack("<Bx", self.dev.ctrl_transfer(0xc0, 0x14, 0, 0, 2, 2000))[0] != 0

    def write_dft(self, data : bytes):
        self.dev.ctrl_transfer(0x40, 0x15, 0, 0, data, 2000)

    def get_event_data(self) -> bytes:
        buf = array.array('B', [0 for _ in range(8)])
        #For some reason, Ctrl+C doesn't work while waiting on an interrupt endpoint
        #So we repeatedly timeout so that KeyboardInterrupts are triggered
        while True:
            try:
                num_read = self.intr_ep.read(buf, 1000)
                break
            except usb.core.USBTimeoutError: pass
        return bytes(buf[:num_read])

class LogCommunicationProxy(CommunicationInterface):
    proxied : CommunicationInterface

    def __init__(self, proxied):
        self.proxied = proxied

    def close(self):
        logging.log(LOG_COMM, "---------------------- CLOSE ---------------------")
        self.proxied.release()

    def reset(self):
        logging.log(LOG_COMM, "---------------------- RESET ---------------------")
        self.proxied.reset()

    def send_command(self, cmd, resp_size, timeout = 2000, raw = False):
        if raw:
            logging.log(LOG_COMM, "-> RAW REQ     | %s" % cmd.hex())
            resp = self.proxied.send_command(cmd, resp_size, timeout, raw)
            logging.log(LOG_COMM, "<- RAW RESP    | %s" % resp.hex())
            return resp
        else:
            logging.log(LOG_COMM, "-> cmd %02x      | %s" % (struct.unpack("<B", cmd[:1])[0], cmd.hex()))
            resp = self.proxied.send_command(cmd, resp_size, timeout, raw)
            logging.log(LOG_COMM, "<- status %04x | %s" % (struct.unpack("<H", resp[:2])[0], resp.hex()))
            return resp

    def set_tls_session(self, session : tudor.tls.TlsSession): 
        if session != None: logging.log(LOG_COMM, "---------- TLS session start ----------")
        else: logging.log(LOG_COMM, "---------- TLS session end ----------")
        self.proxied.set_tls_session(session)

    def remote_tls_status(self):
        logging.log(LOG_COMM, "-> remote TLS session status?")
        status = self.proxied.remote_tls_status()
        logging.log(LOG_COMM, "<- remote TLS session status: %s" % ("established" if status else "not established"))
        return status

    def write_dft(self, data : bytes):
        logging.log(LOG_COMM, "-> DFT write: %s" % data.hex())
        self.proxied.write_dft(data)

    def get_event_data(self) -> bytes:
        logging.log(LOG_COMM, "-> get event data")
        data = self.proxied.get_event_data()
        logging.log(LOG_COMM, "<- event data: %s" % data.hex())
        return data