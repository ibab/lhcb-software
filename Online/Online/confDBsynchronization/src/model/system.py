from subsystem import Subsystem
"""
    System represents the whole systems (with subsystems)
"""
class System(object):
    def __init__(self, name = "System"):
        self.subsystems = []
        self.name = name
    def addSubsystem(self, subsystem):
        self.subsystems.append(subsystem)
    def getAllSubsystems(self):
        return self.subsystems
    def getSubsystem(self, name):
        for subsystem in self.subsystems:
            if subsystem.name == name:
                return subsystem
        return None
    """checks if a subsystem with a specific name already exists"""
    def checkSystem(self, name):
        if len(self.subsystems) == 0:
            return False
        for subsystem in self.subsystems:
            if subsystem.name == name:
                return True
        return False #subsystem with this name does not exist
    """adds a device, will put the device in the appropriate subsystem"""
    def addDevice(self, device):
        if self.checkSystem(device.subsystem.system):
            #subsystem exists
            self.getSubsystem(device.subsystem.system).addDevice(device)
        else:
            #subsystem does not exist yet, so it has to be created
            self.subsystems.append(Subsystem(device.subsystem.system))
            self.getSubsystem(device.subsystem.system).addDevice(device)
    def getAllDevices(self):
        devices = []
        for subsystem in self.subsystems:
            for device in subsystem.getAllDevices():
                devices.append(device)
        return devices
    """searches the hugin which the device will be connected to (throttle)"""
    def getHuginForDevice(self, device):
        if device.hwtyp == "HUGIN":
            return device
        subsystem = self.getSubsystem(device.subsystem.system)
        location_without_slot = device.loc[:-2]
        #Exception for PRS
        if subsystem.name == "PRS":
            #look for a hugin in the same subsystem
            for dev in subsystem.getAllDevices():
                if dev.hwtyp == "HUGIN":
                        return dev
        #global search for hugin in the same crate in slot 21
        for dev in self.getAllDevices():
            if dev.hwtyp == "HUGIN":
                if location_without_slot+"21" == dev.loc:
                    #hugin in the same crate in slot21 was found
                    return dev
        return None
    def __str__(self):
        output = "\n------------------------- "+self.name+"--------------------------------\n"
        if len(self.subsystems) == 0:
            output+= "\n\tNo devices\n"
        for subsystem in self.subsystems:
            output+= "\n  Subsystem "+subsystem.name
            for device in subsystem.getAllDevices():
                output += "\n    "+device.devicename+",\t"+device.loc
        output += "\n------------------------- "+self.name+"--------------------------------\n\n\n"
        return output