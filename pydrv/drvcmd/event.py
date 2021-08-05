from .cmd import *
from .context import *

import tudor.sensor

@cmd("wait_event")
class CmdWaitEvent(Command):
    """
        Waits for a specified sensor event
        For types of events, see tudor/sensor/event.py
        Usage: wait_event <event names>
    """

    def run(self, ctx : CmdContext, args : list):
        if len(args) <= 0: raise Exception("No event name specified!")
        if not ctx.sensor.initialized: raise Exception("Sensor isn't initialized!")

        e = ctx.sensor.event_handler.wait_for_event([tudor.sensor.SensorEventType[n.upper()] for n in args])
        print("Event raised: %r" % e)