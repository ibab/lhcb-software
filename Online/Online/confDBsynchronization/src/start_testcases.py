from testcases.swapdevices import swapDeviceAndDevice
from time import strftime
from string import replace
from string import upper, lower, split
from database.database import ConfigurationDB, EquipmentDB

if __name__ == "__main__":
    datestamp = strftime("%m.%d.%Y - %H:%M:%S")
    datestamp = replace(datestamp, ":", "-")
    logfile = open("log/"+datestamp + " session.log", "w")
    confDB = ConfigurationDB(logfile)
    equipDB = EquipmentDB(logfile)
    if swapDeviceAndDevice(confDB, equipDB):
        print "swap successfull"
    else:
        print "swap not successfull"
    logfile.close()