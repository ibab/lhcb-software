"""
    represents a subsystem (with devices, etc.)
"""
class Subsystem(object):
    def __init__(self, name):
        self.devices = []
        self.name = name
    def addDevice(self, device):
        self.devices.append(device)
    def getAllDevices(self):
        return self.devices