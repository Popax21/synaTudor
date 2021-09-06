from __future__ import annotations

import struct
import logging
import ctypes
import tudor
from .sensor import *
from .event import *

class SensorTudorIplParameters(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        ("pixel_bits", ctypes.c_uint16), 
        ("frame_header_size", ctypes.c_uint16), ("width", ctypes.c_uint16), ("x_off", ctypes.c_uint16), ("x_size", ctypes.c_uint16), 
        ("col_header_size", ctypes.c_uint16), ("height", ctypes.c_uint16), ("y_off", ctypes.c_uint16), ("y_size", ctypes.c_uint16),
        ("__pad1", ctypes.c_uint8*2),
        ("config_ver_major", ctypes.c_uint32), ("config_ver_minor", ctypes.c_uint32),
        ("ipl_type", ctypes.c_uint8),
        ("__pad2", ctypes.c_uint8*2),
        ("iota_size", ctypes.c_uint32), ("__pad3", ctypes.c_uint8*4), ("iota_data", ctypes.POINTER(ctypes.c_uint8))
    ]

class SensorImage:
    def __init__(self, width : int, height : int, data : list = None, enough_coverage : bool = True):
        assert 0 < width and 0 < height
        self.width = width
        self.height = height

        if data != None:
            assert len(data) == width and len(data[0]) == height
            self.data = data
        else: self.data = [[0 for _ in range(height)] for _ in range(width)]

        self.enough_coverage = enough_coverage

    def __getitem__(self, idx : tuple) -> int: return self.data[idx[0]][idx[1]]
    def __setitem__(self, idx : tuple, val : int): self.data[idx[0]][idx[1]] = val

class SensorFrameCapturer:
    IPL_TYPES = {
        0x35: 0,
        0x38: 1,
        0x3a: 2,
        0x3c: 3,
        0x41: 4
    }

    def __init__(self, sensor : Sensor, dim_data : bytes):
        self.sensor = sensor

        #Parse frame dimensions
        self.pixel_bits, self.width, self.frame_header_size, self.x_off, self.x_size, self.height, self.col_header_size, self.y_off, self.y_size = struct.unpack("<HHHHHHHHH", dim_data)
        logging.log(tudor.LOG_PROTO, "Frame dimensions:")
        logging.log(tudor.LOG_PROTO, "    frame header size: %d" % self.frame_header_size)
        logging.log(tudor.LOG_PROTO, "    colum header size: %d" % self.col_header_size)
        logging.log(tudor.LOG_PROTO, "    pixel bits: %d" % self.pixel_bits)
        logging.log(tudor.LOG_PROTO, "    frame size: %dx%d" % (self.width, self.height))
        logging.log(tudor.LOG_PROTO, "    offset: %d,%d" % (self.x_off, self.y_off))
        logging.log(tudor.LOG_PROTO, "    size: %dx%d" % (self.x_size, self.y_size))

        self.frame_stride = self.col_header_size + -(-(self.height * self.pixel_bits) // 32) * 4
        self.frame_size = self.frame_header_size + self.width * self.frame_stride

    def capture_frames(self, num_frames : int) -> list:
        frames = []

        #Start capture
        #TODO Current flags values are pure guesswork
        logging.log(tudor.LOG_PROTO, "Starting frame capture...")
        self.sensor.comm.send_command(struct.pack("<BIIHxBBBBB", tudor.Command.FRAME_ACQ, 0, num_frames, 1, 8, 1, 1, 1, 0), 2)
        
        seq_num = 0
        try: 
            while True:
                #Wait until a frame is available
                logging.log(tudor.LOG_PROTO, "Waiting for new frame...")
                logging.log(tudor.LOG_PROTO, "    current sequence number: %d" % seq_num)
                while True:
                    new_seq_num = self.sensor.comm.get_event_data()[5] & 0x7
                    logging.log(tudor.LOG_PROTO, "    new sequence number: %d" % seq_num)
                    if new_seq_num != seq_num: break

                #Read frame
                logging.log(tudor.LOG_PROTO, "Reading frame seq=%04x" % seq_num)
                resp = self.sensor.comm.send_command(struct.pack("<BHxxHH", tudor.Command.FRAME_READ, seq_num, 0xffff, 3), 8 + self.frame_size)
                seq_num += 1

                frame_flags, frame_idx = struct.unpack("<xxHxxH", resp[:8])
                frame_flags &= 0b11
                logging.log(tudor.LOG_PROTO, "    frame flags: %x" % frame_flags)
                logging.log(tudor.LOG_PROTO, "    frame index: %x" % frame_idx)

                #The finger mustn't be lifted
                if (frame_flags & 2) != 0: raise Exception("Finger lifted")

                #Add frame to output
                frames.append(resp[8:8+self.frame_size])

                #If "last frame" flag is set, exit
                if (frame_flags & 1) != 0: return frames
        finally: 
            self.sensor.comm.send_command(struct.pack("<B", tudor.Command.FRAME_FINISH), 2)

    def capture_images(self, num_images : int) -> list:
        return [self.frame_to_image(f) for f in self.capture_frames(num_images)]

    def frame_to_image(self, frame : bytes) -> SensorImage:
        if self.sensor.product_id != SensorProductId.PROD_ID5: raise Exception("Image conversion not supported for product id %r" % self.sensor.product_id)

        #Load native library
        ln = ctypes.cdll.LoadLibrary(str(pathlib.Path(__file__).parent) + "/libnative/libnative.so")
        ln.process_frame.argtypes = [ctypes.POINTER(SensorTudorIplParameters), ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.POINTER(ctypes.POINTER(ctypes.c_uint8)), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_bool)]
        ln.process_frame.restype = ctypes.c_int

        ln.free_image.argtypes = [ctypes.POINTER(ctypes.c_uint8)]
        ln.free_image.restype = None

        #Process frame
        img_data = ctypes.POINTER(ctypes.c_uint8)()
        img_width, img_height = ctypes.c_int(), ctypes.c_int()
        enough_coverage = ctypes.c_bool()

        try:
            r = ln.process_frame(ctypes.byref(SensorTudorIplParameters(
                    pixel_bits=self.pixel_bits, 
                    frame_header_size=self.frame_header_size, width=self.width, x_off=self.x_off, x_size=self.x_size,
                    col_header_size=self.col_header_size, height=self.height, y_off=self.y_off, y_size=self.y_size,
                    config_ver_major=self.sensor.cfg_ver.major, config_ver_minor=self.sensor.cfg_ver.minor,
                    ipl_type=SensorFrameCapturer.IPL_TYPES[self.sensor.product_id],
                    iota_size=len(self.sensor.ipl_iota.payload), iota_data=(ctypes.c_uint8 * len(self.sensor.ipl_iota.payload)).from_buffer_copy(self.sensor.ipl_iota.payload)
                )), 
                (ctypes.c_ubyte * len(frame)).from_buffer_copy(frame), len(frame), 
                ctypes.byref(img_data), ctypes.byref(img_width), ctypes.byref(img_height), ctypes.byref(enough_coverage)
            )
            if r != 0: raise Exception("Couldn't process frame data: %d" % r)

            #Convert image
            img_width = img_width.value
            img_height = img_height.value
            img = SensorImage(img_width, img_height, [[(int(img_data[x*img_height + y]) + (1 << 7)) % (1 << 8) for y in range(img_height)] for x in range(img_width)], enough_coverage)
        finally:
            #Free image
            if img_data: ln.free_image(img_data)

        return img