from model.Hugin import Hugin



def swapDeviceAndDevice(confDB, equipDB):
    equipDB_system = equipDB.getSystem()
    hugin1 = None
    hugin2 = None
    for device in equipDB_system.getAllDevices():
        if device.typ == "hugin":
            if hugin1 is None:
                hugin1 = device
                print "hugin1 = "+str(device.devicename)
            else:
                hugin2 = device
                print "hugin2 = "+str(device.devicename)
                break #hugins found, now lets test
    print "hugins found, swapping hugins"
    if hugin1 is not None and hugin2 is not None:
        return equipDB.swapDevices(hugin1, hugin2)
    else:
        return False
def swapDeviceAndSpare(confDB):
    pass
def swapSpareAndSpare(confDB):
    pass



