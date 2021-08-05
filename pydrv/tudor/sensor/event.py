from __future__ import annotations

import enum
import struct
import tudor
from .sensor import *

class SensorEventType(enum.IntEnum):
    FINGER_PRESS = 1    #Windows: 0x0080
    FINGER_REMOVE = 2   #Windows: 0x0100
    EVENT3 = 3          #Windows: 0x0001
    EVENT4 = 4          #Windows: 0x0002
    EVENT5 = 5          #Windows: 0x0040
    EVENT6 = 6          #Windows: 0x0004
    EVENT7 = 7          #Windows: 0x0008
    EVENT8 = 8          #Windows: 0x0010
    EVENT9 = 9          #Windows: 0x0020
    EVENT10 = 24        #Windows: 0x1000

class SensorEvent:
    def __init__(self, event_type : SensorEventType):
        self.event_type = event_type

    def __repr__(self): return "SensorEvent(type=%r)" % self.event_type

class SensorEventHandler:
    def __init__(self, sensor : Sensor):
        self.sensor = sensor
        self.legacy_mode = False
        self.event_seq_num = 0
        self.num_pending = 0
        self.event_queue = []

        #Init event mask
        self.event_mask = []
        self.set_event_mask([])

    #NOTE If the event condition is already satisfied, this will immediatly triger a event
    def set_event_mask(self, events : list):
        logging.log(tudor.LOG_PROTO, "Setting event mask to %r..." % events)

        #Create event mask
        event_mask = 0
        for e in events: event_mask |= (1 << e)

        #Send command
        req = struct.pack("<B8II", tudor.Command.EVENT_CONFIG, event_mask, event_mask, event_mask, event_mask, event_mask, event_mask, event_mask, event_mask, 0 if event_mask != 0 else 4)
        resp = self.sensor.comm.send_command(req, 0x42)

        #Get current sequence number
        self.event_seq_num, = struct.unpack("<64xH", resp)
        logging.log(tudor.LOG_PROTO, "    current event sequence number: %04x" % self.event_seq_num)

        self.event_mask = events

    def read_events(self, block : bool = False) -> int:
        if self.num_pending <= 0 and block:
            logging.log(tudor.LOG_PROTO, "Waiting for sensor to have events...")
            while True:
                #Tell communications layer to wait for a event to be pending
                sensor_seq_num, = struct.unpack("<xxxxxxB", self.sensor.comm.get_event_data())
                sensor_seq_num &= 0x1f
                logging.log(tudor.LOG_PROTO, "    event sequence numbers: host=%d sensor=%d" % (self.event_seq_num & 0x1f, sensor_seq_num))
                if sensor_seq_num != self.event_seq_num: break

        #Read sensor events
        logging.log(tudor.LOG_PROTO, "Reading sensor events...")
        while True:
            try:
                #Send command
                req = struct.pack("<BHH", tudor.Command.EVENT_READ, self.event_seq_num, 32) if self.legacy_mode else struct.pack("<BHHI", tudor.Command.EVENT_READ, self.event_seq_num, 32, 1)
                resp = self.sensor.comm.send_command(req, 390)
                num_evts, num_pending = struct.unpack("<xxHH", resp[:6])

                #Parse events
                logging.log(tudor.LOG_PROTO, "    num events: %d" % num_evts)
                for i in range(num_evts):
                    evt_type, = struct.unpack("<Bxxxxxxxxxxx", resp[6 + i*12:6 + (i+1)*12])
                    self.event_queue.append(SensorEvent(SensorEventType(evt_type)))

                #Parse number of pending events
                if self.legacy_mode:
                    assert num_pending >= num_evts
                    self.num_pending = num_pending - num_evts
                else: self.num_pending = num_pending
                logging.log(tudor.LOG_PROTO, "    num pending: %d", self.num_pending)

                #Update event sequence number
                self.event_seq_num = (self.event_seq_num + num_evts) & 0xffff
                logging.log(tudor.LOG_PROTO, "    new event sequence number: 0x%04x" % self.event_seq_num)

                return num_evts
            except tudor.CommandFailedException as e:
                if self.legacy_mode: raise e
                if e.status < 0x405 or 0x407 < e.status: raise e

                #Fallback to legacy mode
                logging.log(tudor.LOG_WARN, "Falling back to legacy error reading mode")
                self.legacy_mode = True

    def iter_events(self, block : bool = False):
        while True:
            #Yield queued events
            while len(self.event_queue) > 0: yield self.event_queue.pop(0)

            #Read new events
            self.read_events(block=block)

            #If we don't block, and there are no events in the queue, exit
            if not block and len(self.event_queue) <= 0: return

    def wait_for_event(self, event_types : list):
        #Set event mask
        self.set_event_mask(event_types)
        try:
            #Wait until we get a event which is in our types list
            for e in self.iter_events(block=True):
                if e.event_type in event_types: return e
                logging.log(tudor.LOG_WARN, "Ignoring sensor event %r" % e)
        finally: self.set_event_mask([])