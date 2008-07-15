from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class InsertDevicesWorker(QThread):
    def __init__(self, serilanbs, equipDB, confDB, db):
        QThread.__init__(self)
        self.serialnbs = serilanbs
        self.equipDB = equipDB
        self.confDb = confDB
        self.db = db
    def run(self):
        print "InsertDevicesWorker.run() start"
        for serial in self.serialnbs:
            insert_exception = False
            device = None
            try:
                device = self.equipDB.getDeviceBySerial(serial)
                print "device found: "+str(device)
                device.insert(self.db, 1, 1)
            except RuntimeError, inst:
                print "error inserting "+str(device)
                print str(inst)
                insert_exception = True
            try:
                if insert_exception == False and device is not None and device.CIPadd is not None:
                    device.insports(self.db)
            except RuntimeError, inst:
                print "error inserting insports for"+str(device)
                print (inst)
            self.emit(QtCore.SIGNAL("progress"))
        self.emit(QtCore.SIGNAL("insertdevicesfinish"))
        print "InsertDevicesWorker.run() end"
