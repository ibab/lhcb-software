from PyQt4.QtCore import QThread
from PyQt4 import QtCore
from string import upper
from database.emptydatabase import removedev

class RemoveAllDevicesWorker(QThread):
    def __init__(self, confDB, db, count):
        QThread.__init__(self)
        self.confDB = confDB
        self.db = db
        self.count = count
    def run(self):
        print "RemoveAllDevicesWorker.run() start"
        workDone = 0
        query = "select devicename from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
        result = self.confDB.executeSelectQuery(query)
        db = self.db
        if db is None or db == False:
            print "No db connection!"
            return
        i = 0
        if len(result) > 1:
            for device in result:
                self.confDB.log("removing "+upper(device[0]))
                try:
                    removedev(db, upper(device[0]))
                except RuntimeError, inst:
                    self.log("could not remove "+upper(device[0])+" "+inst.__str__())
                workDone += 1
                self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
        query = "delete from LHCb_microscopic_connectivity where author like 'DSONNICK'"
        self.confDB.executeQuery(query)
        query = "delete from LHCb_macroscopic_connectivity where author like 'DSONNICK'"
        self.confDB.executeQuery(query)
        query = "delete from LHCB_port_properties where author like 'DSONNICK'"
        self.confDB.executeQuery(query)
        query = "delete from LHCB_IPINFO where IPNAME NOT like 'pctest'"
        self.confDB.executeQuery(query)
        self.emit(QtCore.SIGNAL("removealldevicesdone()"))
        print "RemoveAllDevicesWorker.run() end"