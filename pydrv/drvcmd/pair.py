from .cmd import *
from .context import *

import tudor.sensor

@cmd("load_pdata")
class CmdLoadPairdata(Command):
    """
    Loads pairing data from a file
    Usage: load_pdata <file>
    """

    def run(self, ctx : CmdContext, args : list):
        if len(args) <= 0: raise Exception("No file given!")
        if ctx.pairing_data != None and input("Do you want to override existing pairing data (y/n): ").strip().lower() != "y": return

        with open(" ".join(args), "rb") as f: ctx.pairing_data = tudor.sensor.SensorPairingData.load(f)
        print("Successfully loaded pairing data")

@cmd("save_pdata")
class CmdSavePairdata(Command):
    """
    Saves pairing data to a file
    Usage: save_pdata <file>
    """

    def run(self, ctx : CmdContext, args : list):
        if len(args) <= 0: raise Exception("No file given!")
        if ctx.pairing_data == None: raise Exception("No pairing data present!")

        with open(" ".join(args), "wb") as f: ctx.pairing_data.save(f)
        print("Successfully saved pairing data")

@cmd("pair")
class CmdPair(Command):
    """Pairs the sensor"""

    def run(self, ctx : CmdContext, _):
        if ctx.pairing_data != None and input("Do you want to override existing pairing data (y/n): ").strip().lower() != "y":  return
        ctx.pairing_data = ctx.sensor.pair()
        print("Successfully paired sensor")

@cmd("unpair")
class CmdUnpair(Command):
    """Unpairs the sensor"""

    def run(self, ctx : CmdContext, _):
        if not ctx.sensor.initialized: raise Exception("Sensor isn't initialized!")
        if not ctx.sensor.is_paired(): raise Exception("Sensor isn't paired!")
        if ctx.pairing_data == None: raise Exception("No pairing data present!")
        ctx.sensor.unpair(ctx.pairing_data)
        ctx.pairing_data = None
        print("Successfully unpaired sensor")