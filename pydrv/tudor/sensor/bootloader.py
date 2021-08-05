from __future__ import annotations

import pathlib
import glob
import tudor
import tudor.win
from .sensor import *

class SensorBootloaderPatch:
    data : bytes

    def should_apply(self, sensor : Sensor) -> bool: raise NotImplementedError()
    def get_data(self) -> bytes: raise NotImplementedError()

class SensorBootloaderMFWPatch:
    def __init__(self, ver_major : int, ver_minor : int, ver_build : int, data : bytes):
        self.ver_major = ver_major
        self.ver_minor = ver_minor
        self.ver_build = ver_build
        self.data = data

    def __str__(self): return "MFWPatch(ver_major=%d ver_minor=%d ver_build=%d)" % (self.ver_major, self.ver_minor, self.ver_build)

    def should_apply(self, sensor : Sensor) -> bool:
        return sensor.fw_major == self.ver_major and sensor.fw_minor == self.ver_minor and sensor.fw_build_num < self.ver_build

class SensorBootloaderIOTAPatch:
    def __init__(self, ver_major : int, ver_minor : int, ver_rev : int, data : bytes):
        self.ver_major = ver_major
        self.ver_minor = ver_minor
        self.ver_rev = ver_rev
        self.data = data

    def __str__(self): return "IOTAPatch(ver_major=%d ver_minor=%d ver_ref=%d)" % (self.ver_major, self.ver_minor, self.ver_rev)

    def should_apply(self, sensor : Sensor) -> bool:
        return sensor.cfg_ver.major == self.ver_major and (self.ver_minor == 0 or sensor.cfg_ver.minor == self.ver_minor) and sensor.cfg_ver.revision < self.ver_rev

class SensorFirmwareUpdate:
    def __init__(self, patches : list): self.patches = patches

    @staticmethod
    def load(update_data : list) -> SensorFirmwareUpdate:
        patches = []

        for u in update_data:
            #Decode as tag-value container
            cont = tudor.win.WinTagValContainer.frombytes(u[:-0x100])

            #Check if this update contains MFW patches
            if 1 in cont:
                #Check metadata
                assert len(cont[1]) == 0x18
                sth, ver_build, ver_major, ver_minor = struct.unpack("<IxxxxxxxxIBBxxxxxx", cont[1][:0x18])
                assert sth == 0x41

                #Parse patches
                pdata = cont[2]
                while len(pdata) > 0:
                    patch_size, = struct.unpack("<I", pdata[:4])
                    patches.append(SensorBootloaderMFWPatch(ver_major, ver_minor, ver_build, pdata[4:4+patch_size]))
                    pdata = pdata[4+patch_size:]

            
            #Check if this update contains IOTA patches
            if 3 in cont:
                #Check metadata
                assert len(cont[3]) == 0x10
                sth, ver_major, ver_minor, ver_rev = struct.unpack("<IIIHxx", cont[3][:0x10])
                assert sth == 0x41

                #Parse patches
                pdata = cont[4]
                while len(pdata) > 0:
                    patch_size, = struct.unpack("<I", pdata[:4])
                    patches.append(SensorBootloaderIOTAPatch(ver_major, ver_minor, ver_rev, pdata[4:4+patch_size]))
                    pdata = pdata[4+patch_size:]

        assert len(patches) > 0
        return SensorFirmwareUpdate(patches)

    @staticmethod
    def load_dir(path : str):
        #Load all *.supd files in the firmware directory
        update_data = []
        for fn in glob.iglob("%s/*.tupd" % path):
            with open(fn, "rb") as f: update_data.append(f.read())
        return SensorFirmwareUpdate.load(update_data)

    @staticmethod
    def load_builtin(): return SensorFirmwareUpdate.load_dir(str(pathlib.Path(__file__).parent) + "/builtin_fw")
    
class SensorBootloader:
    def __init__(self, sensor : Sensor):
        self.sensor = sensor

    def enter_bootloader_mode(self):
        if self.sensor.in_bootloader_mode(): return 
        logging.log(tudor.LOG_DETAIL, "Entering bootloader mode...")

        if self.sensor.initialized: self.sensor.uninitialize()
        self.sensor.comm.write_dft(bytes([0, 0, 0, 0, 0, 0, 1, 0]))
        self.sensor.reset()
        
    def exit_bootloader_mode(self):
        if not self.sensor.in_bootloader_mode(): return 
        logging.log(tudor.LOG_DETAIL, "Exiting bootloader mode...")
        
        self.sensor.comm.write_dft(bytes([0, 0, 0, 0, 0, 0, 0, 0]))
        self.sensor.reset()
    
    def update_firmware(self, update : SensorFirmwareUpdate):
        logging.log(tudor.LOG_INFO, "Updating firmware...")

        #If we're in bootloader mode, exit it
        if self.sensor.in_bootloader_mode(): self.exit_bootloader_mode()

        #If we're still in bootloader mode, that means the sensor doesn't have a valid firmware
        bypass_checks = False
        if self.sensor.in_bootloader_mode():
            logging.log(tudor.LOG_WARN, "Sensor doesn't have a valid firmware!")
            bypass_checks = True

        #Apply MFW patches
        logging.log(tudor.LOG_DETAIL, "Applying MFW patches...")
        self.enter_bootloader_mode()
        try:
            for p in update.patches:
                if isinstance(p, SensorBootloaderMFWPatch):
                    if bypass_checks or p.should_apply(self.sensor):
                        #Apply patch
                        logging.log(tudor.LOG_DETAIL, "    Applying MFW patch %s..." % p)
                        self.sensor.comm.send_command(struct.pack("<B", tudor.Command.BOOTLOADER_PATCH) + p.data, 2, timeout=12000)
                    else: logging.log(tudor.LOG_DETAIL, "    Skipping MFW patch %s" % p)
        finally: self.exit_bootloader_mode()
        
        #Apply IOTA patches
        logging.log(tudor.LOG_DETAIL, "Applying IOTA patches...")
        self.enter_bootloader_mode()
        try:
            for p in update.patches:
                if isinstance(p, SensorBootloaderIOTAPatch):
                    if p.should_apply(self.sensor):
                        #Apply patch
                        logging.log(tudor.LOG_DETAIL, "    Applying IOTA patch %s..." % p)
                        self.sensor.comm.send_command(struct.pack("<B", tudor.Command.BOOTLOADER_PATCH) + p.data, 2)
                    else: logging.log(tudor.LOG_DETAIL, "    Skipping IOTA patch %s" % p)
        finally: self.exit_bootloader_mode()

        logging.log(tudor.LOG_INFO, "Firmware update successfull")
        
    def create_partition(self):
        logging.log(tudor.LOG_INFO, "Creating storage partition...")
    
        #If we're in bootloader mode, exit it
        if self.sensor.in_bootloader_mode(): self.exit_bootloader_mode()
        assert not self.sensor.in_bootloader_mode()

        #Load the partition patch
        with open(str(pathlib.Path(__file__).parent) + "/builtin_fw/partition.tptch", "rb") as f:
            part_patch = f.read()
        
        #Apply partition patch
        self.enter_bootloader_mode()
        try: self.sensor.comm.send_command(struct.pack("<B", tudor.Command.BOOTLOADER_PATCH) + part_patch, 2)
        finally: self.exit_bootloader_mode()
        
        logging.log(tudor.LOG_INFO, "Storage partition creation sucessfull")