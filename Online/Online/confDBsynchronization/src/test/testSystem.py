from database.database import ConfigurationDB, EquipmentDB

confDB = ConfigurationDB()
equipDB = EquipmentDB()

system = equipDB.getSystem()

print "System"
for subsystem in system.getAllSubsystems():
    print "  Subsystem "+subsystem.name
    for device in subsystem.getAllDevices():
        hugin = system.getHuginForDevice(device)
        if hugin is not None:
            hugin_name = hugin.devicename
            hugin_location = hugin.loc
        else:
            hugin_name = "Hugin not found"
            hugin_location = ""
        print "    "+device.devicename+",\t"+device.loc+"\tHugin:\t"+hugin_name+"\t"+hugin_location
    print "\n"