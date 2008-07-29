from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class InsertDevicesWorker(QThread):
    def __init__(self, serilanbs, equipDB, confDB, db):
        print "InsertDevicesWorker.__init__() start"
        QThread.__init__(self)
        self.serialnbs = serilanbs
        self.equipDB = equipDB
        self.confDB = confDB
        self.db = db
        print "InsertDevicesWorker.__init__() end"
    def run(self):
        print "InsertDevicesWorker.run() start"
        workDone = 0
        equipDBSystem = self.equipDB.getSystem()
        for serial in self.serialnbs:
            workDone += 1
            insert_exception = False
            device = None
            try:
                device = self.equipDB.getDeviceBySerial(serial)
                #print "device found: "+str(device)
                device.insert(self.db, 1, 1)
                self.confDB.log("inserted "+str(device))
            except RuntimeError, inst:
                print "error inserting "+str(device)
                self.confDB.log("error inserting "+str(device)+"\n"+str(inst))
                print str(inst)
                insert_exception = True
            try:
                if insert_exception == False and device is not None and device.CIPadd is not None:
                    device.insports(self.db)
                    self.confDB.log("inserted ports for "+str(device)+"\n")
            except RuntimeError, inst:
                print "error inserting insports for"+str(device)
                print (inst)
                self.confDB.log("error inserting ports for "+str(device)+"\n"+str(inst))
            self.emit(QtCore.SIGNAL("workProgressed(int)"), workDone)
        try:
            self.confDB.createThrottleConnectivity(self.db, equipDBSystem.getAllDevices(), equipDBSystem)
        except RuntimeError, inst:
            print "error creating throttle connectivity for "+str(device.devicename)+"\n"+str(inst)
        self.confDB.insert_MUNIN(self.db)
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "InsertDevicesWorker.run() end"
