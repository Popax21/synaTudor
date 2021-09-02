from .cmd import *
from .context import *

@cmd("info")
class CmdInfo(Command):
    """Prints info about the driver and sensor"""

    def run(self, ctx : CmdContext, _):
        #Print driver info
        print("Driver info:")
        print("    pairing data: %s" % ("present" if ctx.pairing_data != None else "not present"))
        print("    initialized: %s" % ("yes" if ctx.sensor.initialized else "no"))
        print("    in TLS session: %s" % ("yes" if ctx.sensor.tls_session != None else "no"))
        
        print()

        #Print sensor info
        print("Sensor info:")
        print("    ID: %s" % ctx.sensor.id.hex())
        print("    FW version: %d.%d.%d" % (ctx.sensor.fw_major, ctx.sensor.fw_minor, ctx.sensor.fw_build_num))
        print("    product id: %s" % ctx.sensor.product_id)
        if not ctx.sensor.in_bootloader_mode():
            print("    advanced security: %s" % ("present" if ctx.sensor.advanced_security else "not present"))
            print("    key flag: %s" % ("set" if ctx.sensor.key_flag else "not set"))
            print("    provision state: %s" % ctx.sensor.prov_state)
            print("    config version: %d.%d.%d" % (ctx.sensor.cfg_ver.major, ctx.sensor.cfg_ver.minor, ctx.sensor.cfg_ver.revision))
            print("    WBF parameter: 0x%x" % ctx.sensor.wbf_param_iota.param)
        if ctx.sensor.initialized:
            print("    event sequence number: %04x" % ctx.sensor.event_handler.event_seq_num)
