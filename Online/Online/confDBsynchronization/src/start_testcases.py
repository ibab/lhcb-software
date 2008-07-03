from testcases.swapdevicesconfdb import SwapDevicesConfDB
from testcases.swapdeviceslhcbintegration import SwapDevicesLHCBIntegration
from time import strftime
from string import replace
from string import upper, lower, split
from database.database import ConfigurationDB, EquipmentDB
from database.spare import SpareDB

if __name__ == "__main__":
    datestamp = strftime("%m.%d.%Y - %H:%M:%S")
    datestamp = replace(datestamp, ":", "-")
    logfile = open("log/"+datestamp + " session.log", "w")
    confDB = ConfigurationDB(logfile)
    equipDB = EquipmentDB(logfile)
    spareDB = SpareDB(confDB, equipDB, logfile)
    print "--- starting testcases ---"
    swapDevicesConfDB = SwapDevicesConfDB(confDB, equipDB, spareDB)
    swapDevicesConfDB.doAllTestCases()
    
    swapDevicesLHCBIntegration = SwapDevicesLHCBIntegration(equipDB)
    swapDevicesLHCBIntegration.doAllTestCases()
    print "--- all testcases finished ---"
    logfile.close()