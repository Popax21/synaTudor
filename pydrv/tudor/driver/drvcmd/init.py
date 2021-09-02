from .cmd import *
from .context import *

@cmd("init")
class CmdInit(Command):
    """Initializes the sensor"""

    def run(self, ctx : CmdContext, _):
        if ctx.sensor.initialized: raise Exception("Sensor is already initialized!")
        ctx.sensor.initialize(ctx.pairing_data)

@cmd("uninit")
class CmdUninit(Command):
    """Uninitializes the sensor"""

    def run(self, ctx : CmdContext, _):
        if not ctx.sensor.initialized: raise Exception("Sensor isn't initialized!")
        ctx.sensor.uninitialize()