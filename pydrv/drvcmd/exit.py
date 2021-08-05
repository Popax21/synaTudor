from .cmd import *
from .context import *

@cmd("exit")
class CmdExit(Command):
    "Exits the program"

    def run(self, ctx : CmdContext, _):
        ctx.exit_loop = True