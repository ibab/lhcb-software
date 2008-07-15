from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class RefreshWorker(QThread):
    def __init__(self, equipDB, confDB, spareDB, devicesInfo, sparesInfo, parentController):
        QThread.__init__(self)
        self.equipDB = equipDB
        self.confDB = confDB
        self.spareDB = spareDB
        self.devicesInfo = devicesInfo
        self.sparesInfo = sparesInfo
        self.parentController = parentController
        #self.connect(self, QtCore.SIGNAL("NewDevicesWithDHCPData(int)"), self.sparesInfo, QtCore.SLOT("setNewDevicesWithDHCPData(*QObject, int)"))
        #self.connect(self, QtCore.SIGNAL("NewDevicesWithoutDHCPData(int)"), self.sparesInfo, QtCore.SLOT("setNewDevicesWithoutDHCPData(*QObject, int)"))
        #self.connect(self, QtCore.SIGNAL("ChangedDevices(int)"), self.sparesInfo, QtCore.SLOT("setChangedDevices(*QObject, int)"))
        #self.connect(self, QtCore.SIGNAL("DevicesUpToDate(int)"), self.sparesInfo, QtCore.SLOT("DevicesUpToDate(*QObject, int)"))
        self.connect(self, QtCore.SIGNAL("devicesInfoChanged()"), self.parentController.onRefreshDevicesWidget)
        self.connect(self, QtCore.SIGNAL("sparesInfoChanged()"), self.parentController.onRefreshSparesWidget)
    def run(self):
        print "RefreshWorker.run() start"
        equipDBSystem = self.equipDB.getSystem()
        changedSystem = self.confDB.getChangedSystems(equipDBSystem)
        #devicesInfo = DevicesInfo()
        self.devicesInfo.setNewDevicesWithDHCPData(len(changedSystem[0].getAllDevices()))
        self.devicesInfo.setNewDevicesWithoutDHCPData(len(changedSystem[1].getAllDevices()))
        self.devicesInfo.setChangedDevices(len(changedSystem[2].getAllDevices())+len(changedSystem[3].getAllDevices()))
        self.devicesInfo.setDevicesUpToDate(len(changedSystem[4].getAllDevices()))
        #self.emit(QtCore.SIGNAL("NewDevicesWithDHCPData(int)"), len(changedSystem[0].getAllDevices()))
        #self.emit(QtCore.SIGNAL("NewDevicesWithoutDHCPData(int)"), len(changedSystem[0].getAllDevices()))
        #self.emit(QtCore.SIGNAL("ChangedDevices(int)"), len(changedSystem[0].getAllDevices()))
        #self.emit(QtCore.SIGNAL("DevicesUpToDate(int)"), len(changedSystem[0].getAllDevices()))
        self.emit(QtCore.SIGNAL("devicesInfoChanged()"))
        self.sparesInfo.setSparesInEquipDB = (self.spareDB.getAllSparesEquipDBCount())
        self.sparesInfo.setSparesInConfDB(self.spareDB.getAllSparesConfDBCount())
        self.sparesInfo.setChangedSpares(len(self.spareDB.getChangedSpares()))
        self.sparesInfo.setNewSpares(len(self.spareDB.getNewSpares()))
        self.emit(QtCore.SIGNAL("sparesInfoChanged()"))
        print "RefreshWorker.run() end"