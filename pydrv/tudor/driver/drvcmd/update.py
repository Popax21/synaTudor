from .cmd import *
from .context import *

import tudor.sensor

@cmd("update")
class CmdUpdate(Command):
    """
    Updates the sensor's firmware
    Usage:
        update builtin
        update fwdir <folder>
        update ufiles <update files>
    """

    def run(self, ctx : CmdContext, args : list):
        if len(args) <= 0: raise Exception("No mode specified")
        if args[0].lower() == "builtin":
            ctx.sensor.bootloader.update_firmware(tudor.sensor.SensorFirmwareUpdate.load_builtin())
            print("Successfully updated to the Windows driver's builtin firmware")
        elif args[0].lower() == "fwdir":
            if len(args) <= 1: raise Exception("No firmware folder specified!")
            ctx.sensor.bootloader.update_firmware(tudor.sensor.SensorFirmwareUpdate.load_dir(" ".join(args[1])))
            print("Successfully updated to specified firmware")
        elif args[0].lower() == "ufiles":
            if len(args) <= 1: raise Exception("At least one update file needs to be specified!")

            #Load update files
            udata = []
            for fn in args:
                with open(fn, "rb") as f: udata.append(f.read())

            ctx.sensor.bootloader.update_firmware(tudor.sensor.SensorFirmwareUpdate.load(udata))
            print("Successfully applied specified update files")
        else: raise Exception("Invalid mode")