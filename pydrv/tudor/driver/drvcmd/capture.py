from .cmd import *
from .context import *

import matplotlib.pyplot as plt
import tudor.sensor

@cmd("capture_images")
class CmdCaptureImages(Command):
    """
    Captures images from the sensor
    Usage: capture_images <number of images>
    """

    def run(self, ctx : CmdContext, args : list):
        if len(args) <= 0: raise Exception("No number of images provided")
        if not ctx.sensor.initialized: raise Exception("Sensor isn't initialized!")
        num_images = int(args[0])
        
        #Wait for finger to be lifted
        print("Waiting for finger to be lifted...")
        ctx.sensor.event_handler.wait_for_event([tudor.sensor.SensorEventType.FINGER_REMOVE])

        #Capture images
        print("Capturing images...")
        images = ctx.sensor.frame_capturer.capture_images(num_images)

        #Show images
        for i in range(len(images)):
            plt.figure("Image %d%s" % (i+1, "" if images[i].enough_coverage else " - INSUFFICIENT COVERAGE"))
            plt.imshow([[images[i][x,y] for x in range(images[i].width)] for y in range(images[i].height)], vmin=0, vmax=0xff, cmap="viridis")
            plt.axis("off")
            plt.show(block=False)