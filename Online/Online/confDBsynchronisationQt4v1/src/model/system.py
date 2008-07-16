from subsystem import Subsystem
from re import compile, match
import re
from string import upper
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
    def getMasterHugin(self, device):
        devicename = device.devicename
        patterns = {}
        ####################################
        vehugina01_pattern = compile(r'vehugina0[2-9]{1}', re.IGNORECASE) #vehugina02, 03 ... 09
        patterns["vehugina01"] = vehugina01_pattern
        vetella_pattern1 = compile(r'vetella((1[0-6]){1}|(0[0-9]){1})', re.IGNORECASE) #vetella01-16
        patterns["vehugina02"] = vetella_pattern1
        vetella_pattern2 = compile(r'vetella((1[7-9]){1}|(2[0-9]){1})', re.IGNORECASE) #vetella01-16
        patterns["vehugina03"] = vetella_pattern2
        vetella_pattern3 = compile(r'vetella((3[0-9]){1}|(4[0-2]){1})', re.IGNORECASE) #vetella01-16
        patterns["vehugina04"] = vetella_pattern3
        vehuginc01_pattern = compile(r'vehuginc(0[2-9]){1}', re.IGNORECASE) #vehugina02, 03 ... 09
        patterns["vehuginc01"] = vehuginc01_pattern
        vetellc_pattern1 = compile(r'vetellc(0[1-8]){1}', re.IGNORECASE)
        patterns["vehuginc02"] = vetellc_pattern1
        vetellc_pattern2 = compile(r'vetellc((09){1}|(1[0-9]){1}|(2[0-5]){1})', re.IGNORECASE)
        patterns["vehuginc03"] = vetellc_pattern2
        vetellc_pattern3 = compile(r'vetellc((2[6-9]){1}|(3[0-9]){1}|(4[0-2]){1})', re.IGNORECASE)
        patterns["vehuginc04"] = vetellc_pattern3
        for (masterhugin, pattern) in patterns.iteritems():
            if pattern.match(device.devicename):
                for hugin_candidate in self.getAllDevices():
                    if hugin_candidate.hwtyp == "HUGIN":
                        if upper(hugin_candidate.devicename) == upper(masterhugin):
                            print "Masterhugin for "+str(device.devicename)+"="+str(hugin_candidate.devicename)
                            return hugin_candidate
        #still no hugin found? try it with the algorithm
        print "no masterhugin found yet, trying the algorithmic way"
        masterhugin = self.getHuginForDevice(device)
        if masterhugin is not None:
            print "Masterhugin for "+str(device.devicename)+"="+str(masterhugin.devicename)
        else:
            print "no masterhugin found for "+str(device.devicename)
        return masterhugin
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

