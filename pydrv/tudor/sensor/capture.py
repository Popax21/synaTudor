from __future__ import annotations

import struct
import logging
import tudor
from .sensor import *
from .event import *

class SensorImage:
    def __init__(self, width : int, height : int, data : list = None):
        assert 0 < width and 0 < height
        self.width = width
        self.height = height

        if data != None:
            assert len(data) == width and len(data[0]) == height
            self.data = data
        else: self.data = [[0 for _ in range(height)] for _ in range(width)]

    def __getitem__(self, idx : tuple) -> int: return self.data[idx[0]][idx[1]]
    def __setitem__(self, idx : tuple, val : int): self.data[idx[0]][idx[1]] = val

    def rotate(self):
        nimg = SensorImage(self.height, self.width)
        for x in range(nimg.width):
            for y in range(nimg.height):
                nimg[x,y] = self[y,x]
        return nimg

class SensorFrameCapturer:
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

        #Parse frame as image
        assert self.pixel_bits == 16
        img = SensorImage(self.width, self.height)
        for x in range(self.width):
            for y in range(self.height):
                o = self.frame_header_size + self.frame_stride * (self.x_off + x) + self.col_header_size + 2 * (self.y_off + y)
                img[x,y] = int.from_bytes(frame[o:o+2], "little")

        #If width is greater than height, rotate image
        if img.width > img.height: img = img.rotate()

        return img