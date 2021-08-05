import tudor.sensor

class CmdContext:
    pairing_data : tudor.sensor.SensorPairingData
    
    def __init__(self, sensor : tudor.sensor.Sensor):
        self.exit_loop = False
        self.sensor = sensor
        self.pairing_data = None