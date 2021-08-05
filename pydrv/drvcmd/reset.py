from .cmd import *
from .context import *

import tudor.sensor

@cmd("reset")
class CmdReset(Command):
    """Resets the sensor"""

    def run(self, ctx : CmdContext, _):
        ctx.sensor.reset()