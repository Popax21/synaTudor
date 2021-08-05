from __future__ import annotations

import enum
import types
import traceback
import logging
import pathlib
import struct
import os
import cryptography.hazmat.primitives.asymmetric.ec as ecc
import cryptography.hazmat.primitives.hashes as hashes
import tudor
import tudor.tls
from .iota import *
from .pair import *

def load_sensor_key(fw_major : int, fw_minor : int, key_flag : bool) -> ecc.EllipticCurvePublicKey:
    #Create sensor key name
    key_name = "%d.%d%s" % (fw_major, fw_minor, "-kf" if key_flag else "")

    #Create path
    path = str(pathlib.Path(__file__).parent)
    path += "/sensor_keys/"
    path += "%s.tsk" % key_name

    #Load key from path
    if not os.path.isfile(path):
        raise Exception("No sensor key '%s' found" % key_name)

    with open(path, "rb") as f:
        key = f.read()

    #Parse x and y (ECC public key)
    key_x = int.from_bytes(key[0:0x44], "little")
    key_y = int.from_bytes(key[0x44:0x88], "little")
    
    logging.log(tudor.LOG_PROTO, "Loaded sensor key '%s'" % key_name)
    return ecc.EllipticCurvePublicNumbers(key_x, key_y, ecc.SECP256R1()).public_key()

class SensorProductId(enum.IntEnum):
    PROD_ID1 = ord('5')
    PROD_ID2 = ord('8')
    PROD_ID3 = ord(':')
    PROD_ID4 = ord('<')
    PROD_ID5 = ord('A')
    BOOTLOADER_A = ord('B')
    BOOTLOADER_B = ord('C')

class SensorProvisionState(enum.IntEnum):
    UNPROVISIONED_A = 0
    UNPROVISIONED_B = 1
    PROVISIONED = 3

    @staticmethod
    def is_provisioned(state):
        return not (state == SensorProvisionState.UNPROVISIONED_A or state == SensorProvisionState.UNPROVISIONED_B)

class Sensor:
    IDLE_TIMEOUT = 200

    def __init__(self, comm : tudor.CommunicationInterface):
        self.comm = comm
        self.bootloader = tudor.sensor.SensorBootloader(self)
        self.tls_session = None
        self.event_handler = None
        self.initialized = False

        #Initial reset of the sensor
        self.reset()

        #If we're in bootloader mode, exit it
        if self.in_bootloader_mode():
            logging.log(tudor.LOG_INFO, "Sensor is in bootloader mode, exiting it...")
            self.bootloader.exit_bootloader_mode()
            if self.in_bootloader_mode():
                logging.log(tudor.LOG_WARN, "Sensor doesn't have a valid firmware, need to update to a valid one first!")

    def reset(self):
        #Uninitialize sensor
        if self.initialized: self.uninitialize()

        logging.log(tudor.LOG_DETAIL, "Resetting sensor...")

        #Reset the sensor
        self.comm.reset()

        #Get the sensor state
        state_data = self.comm.send_command(struct.pack("<B", tudor.Command.GET_VERSION), 0x26)
        self.fw_build_num, self.fw_major, self.fw_minor, product_id, self.id, flags1, flags2, prov_state = struct.unpack("<2xxxxxIBBxbxxxx6sbbxxxxxxxxxxxB", state_data)
        self.id += bytes(2)
        self.advanced_security = (flags1 & 1) != 0
        self.key_flag = (flags2 & 0x20) != 0

        self.product_id = SensorProductId(product_id)
        self.prov_state = SensorProvisionState(prov_state & 0xf)

        if not self.in_bootloader_mode():
            #Read config version & some other IOTAs
            self.cfg_ver = ConfigVersionIOTA.read_from_comm(self.comm)
            self.ipl_iota = IplIOTA.read_from_comm(self.comm)
            self.iota_2e = PackedIOTA.read_from_comm(self.comm, 0x2e)
            self.wbf_param_iota = WbfParamIOTA.read_from_comm(self.comm)

            #Load the sensor key
            self.pub_key = load_sensor_key(self.fw_major, self.fw_minor, self.key_flag)

    def initialize(self, pairing_data : SensorPairingData):
        assert not self.initialized

        #Reset the sensor
        self.reset()

        logging.log(tudor.LOG_INFO, "Initializing sensor...")

        #We mustn't be in bootloader mode
        assert not self.in_bootloader_mode()

        #Log sensor info
        logging.log(tudor.LOG_DETAIL, "Sensor info:")
        logging.log(tudor.LOG_DETAIL, "    ID: %s" % self.id.hex())
        logging.log(tudor.LOG_DETAIL, "    FW version: %d.%d.%d" % (self.fw_major, self.fw_minor, self.fw_build_num))
        logging.log(tudor.LOG_DETAIL, "    advanced security: %s" % ("present" if self.advanced_security else "not present"))
        logging.log(tudor.LOG_DETAIL, "    key flag: %s" % ("set" if self.key_flag else "not set"))
        logging.log(tudor.LOG_DETAIL, "    product id: %s" % self.product_id)
        logging.log(tudor.LOG_DETAIL, "    provision state: %s" % self.prov_state)
        logging.log(tudor.LOG_DETAIL, "    config version: %d.%d.%d" % (self.cfg_ver.major, self.cfg_ver.minor, self.cfg_ver.revision))
        logging.log(tudor.LOG_DETAIL, "    WBF parameter: 0x%x" % self.wbf_param_iota.param)

        #Read and log start info
        start_data = self.comm.send_command(struct.pack("<B", tudor.Command.GET_START_INFO), 0x44)
        start_type, reset_type, start_code, sanity_panic, sanity_code, reset_nvinfo = struct.unpack("<2xBBIII52s", start_data)
        logging.log(tudor.LOG_DETAIL, "Start data:")
        logging.log(tudor.LOG_DETAIL, "    start type: 0x%02x" % start_type)
        logging.log(tudor.LOG_DETAIL, "    reset type: 0x%02x" % reset_type)
        logging.log(tudor.LOG_DETAIL, "    start code: 0x%x" % start_code)
        logging.log(tudor.LOG_DETAIL, "    sanity panic: 0x%x" % sanity_panic)
        logging.log(tudor.LOG_DETAIL, "    sanity code: 0x%x" % sanity_code)
        for i in range(13): logging.log(tudor.LOG_DETAIL, "    reset nvinfo [%02d]: 0x%08x" % (i, struct.unpack("<I", reset_nvinfo[4*i:4*i+4])[0]))

        if self.is_paired():
            if pairing_data is None: raise Exception("No pairing data given")

            #Verify sensor certificate
            self.pub_key.verify(pairing_data.sensor_cert.signature, pairing_data.sensor_cert.signbytes(), ecc.ECDSA(hashes.SHA256()))

            #Establish session
            self.tls_session = tudor.tls.TlsSession(self.comm, tudor.tls.TlsEccRemoteKey(pairing_data.priv_key, pairing_data.host_cert, pairing_data.sensor_cert))
            self.tls_session.establish()
            self.comm.set_tls_session(self.tls_session)
        else: logging.log(tudor.LOG_INFO, "Sensor is unprovisioned, not establishing TLS session...")

        #Get frame dimensions
        dim_data, = struct.unpack("<14x18s2x", self.comm.send_command(struct.pack("<HxxxxxBB", tudor.Command.FRAME_STATE_GET, 2, 7), 0x22))

        #Create event handler
        self.event_handler = tudor.sensor.SensorEventHandler(self)

        #Create frame capturer
        self.frame_capturer = tudor.sensor.SensorFrameCapturer(self, dim_data)

        logging.log(tudor.LOG_INFO, "Sucessfully initialized sensor")
        self.initialized = True

    def uninitialize(self):
        assert self.initialized
        logging.log(tudor.LOG_INFO, "Uninitializing sensor...")

        #TODO Stop frame capturer

        #Stop event handler
        if self.event_handler.event_mask != []: self.event_handler.set_event_mask([])
        self.event_handler = None

        #Close TLS session
        if self.tls_session != None: 
            self.comm.set_tls_session(None)
            self.tls_session.close()
            self.tls_session = None

        self.initialized = False

    def in_bootloader_mode(self): return self.product_id == SensorProductId.BOOTLOADER_A or self.product_id == SensorProductId.BOOTLOADER_B

    def is_paired(self):
        return SensorProvisionState.is_provisioned(self.prov_state)

    def pair(self) -> SensorPairingData:
        #Reset sensor
        self.reset()
        if self.prov_state != 3: raise Exception("Pairing not needed")
        assert self.advanced_security

        logging.log(tudor.LOG_INFO, "Pairing sensor...")

        #Create keypair
        priv_key = ecc.generate_private_key(ecc.SECP256R1())

        #Create certificate and send it to sensor
        host_cert = SensorCertificate.create_host_cert(priv_key.public_key())
        resp = self.comm.send_command(struct.pack("<B", tudor.Command.PAIR) + host_cert.tobytes(), 0x322)

        #Get new host certificate and device certificate
        host_cert, dev_cert = SensorCertificate.frombytes(resp[2:402]), SensorCertificate.frombytes(resp[402:802])

        #Return paring data
        return SensorPairingData(priv_key, host_cert, dev_cert)

    def unpair(self, data : SensorPairingData):
        assert self.initialized
        assert self.is_paired()

        logging.log(tudor.LOG_INFO, "Unpairing sensor...")

        #Reset sensor
        #That's all the Windows driver does :/
        self.reset()

    def __enter__(self): return self
    def __exit__(self, t, v, tb):
        if self.in_bootloader_mode(): self.bootloader.exit_bootloader_mode()
        if self.initialized: self.uninitialize()