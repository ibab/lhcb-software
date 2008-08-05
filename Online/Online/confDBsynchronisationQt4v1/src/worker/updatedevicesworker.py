from PyQt4.QtCore import QThread
from PyQt4 import QtCore, QtGui

class UpdateDevicesWorker(QThread):
    def __init__(self, confDB, equipDB, db):
        QThread.__init__(self)
        self.confDB = confDB
        self.equipDB = equipDB
        self.db = db
    def run(self):
        print "UpdateDevicesWorker.run() start"
        equipDBSystem = self.equipDB.getSystem()
        changedSystem = self.confDB.getChangedSystems(equipDBSystem)
        changedLocationSystem = changedSystem[2]
        changedDHCPDataSystem = changedSystem[3]
        changedSystem_len = len(changedDHCPDataSystem.getAllDevices())
        changedLocationSystem_len = len(changedLocationSystem.getAllDevices())
        print "found "+str(changedSystem_len)+" devices with changed dhcp data"
        print "found "+str(changedLocationSystem_len)+" devices with changed location attribute"
        changesCount = changedSystem_len + changedLocationSystem_len
        if changesCount == 0:
            self.emit(QtCore.SIGNAL("workProgressed(int)"), 25)
            self.emit(QtCore.SIGNAL("workProgressed(int)"), 50)
            self.emit(QtCore.SIGNAL("workProgressed(int)"), 75)
            self.emit(QtCore.SIGNAL("workProgressed(int)"), 100)
            self.emit(QtCore.SIGNAL("workFinished()"))
            return
        factor = int(100/changesCount)
        print "factor: "+str(factor)
        workDone = 0
        for device in changedLocationSystem.getAllDevices():
            self.confDB.updateLocationOfDevice(device, self.db)
            workDone += factor
            self.emit(QtCore.SIGNAL("workProgressed(int)"), workDone)
        for device in changedDHCPDataSystem.getAllDevices():
            self.confDB.updateDHCPDataOfDevice(device, self.db)
            workDone += factor
            self.emit(QtCore.SIGNAL("workProgressed(int)"), workDone)
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "UpdateAllDevicesWorker.run() end"