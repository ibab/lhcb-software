from PyQt4.QtCore import QThread
from PyQt4 import QtCore
from string import upper

class RemoveAllDevicesWorker(QThread):
    def __init__(self, confDB, db, count, devicenames = None):
        QThread.__init__(self)
        self.confDB = confDB
        self.db = db
        self.count = count
        self.devicenames = devicenames
    def run(self):
        print "RemoveAllDevicesWorker.run() start"
        workDone = 0
        db = self.db
        if self.devicenames is None:
            query = "select devicename, deviceid from lhcb_lg_devices where devicetypeid = 644 or devicetypeid = 645"
            result = self.confDB.executeSelectQuery(query)
            if len(result) == 0:
                print "no devices found"
                self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
                return
            else:
                print "found "+str(len(result))+" devices to delete"
                if db is None or db == False:
                    print "No db connection!"
                    self.emit(QtCore.SIGNAL("removealldevicesdone()"))
                    return
                i = 0
        else:
            result = []
            for devicename in self.devicenames:
                result.append([devicename])
        if len(result) > 0:
            for device in result:
                workDone += 1
                self.confDB.log("trying to remove: "+upper(device[0]))
                devid = -1
                try:
                    devid  = db.GetDeviceID_devicename(device[0])
                    #print "devid "+str(devid)
                except RuntimeError, inst:
                    print "Exception retrieving deviceID. Reason: "+str(inst)
                    self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
                    continue
                if devid == -1:
                    print "Device Name not found"
                    self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
                    continue
                #delete history
                self.confDB.executeQuery("DELETE FROM LHCB_DEVICE_HISTORY WHERE deviceid = "+str(devid))
                #delete throttle connection
                self.confDB.deleteThrottleConnectivityStrings([device[0]])
                ports = []
                try:
                    ports = db.GetPortIDPerDevID(devid)
                except RuntimeError, inst:
                    print "error getting ports: "+inst.__str__()
                    self.confDB.log("error getting ports: "+inst.__str__())
                    self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
                #delete microscopic links
                for p in ports:
                    self.confDB.deleteMicroscopicLinkRow(p)
                for p in ports:
                    try:
                        db.DeletePortRow(p)
                        self.confDB.log("deleted port: "+str(p))
                    except RuntimeError, inst:
                        print "port "+str(p)+" for "+nam+" were not deleted! Reason: "+inst.__str__()
                        self.confDB.log("port "+str(p)+" for "+nam+" were not deleted! Reason: "+inst.__str__())
                        self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
                        continue
                try:
                    db.DeleteFunctionalDevice(devid)
                    self.confDB.log("deleted "+str(device[0]))
                except RuntimeError, inst:
                    print "Error while deleteing "+str(device[0])+"! Reason: "+inst.__str__()
                    self.confDB.log("Error while deleteing "+str(devid)+"! Reason: "+inst.__str__())
                self.emit(QtCore.SIGNAL("setValue(int)"), workDone)
        #query = "delete from LHCb_microscopic_connectivity where author like 'DSONNICK'"
        #self.confDB.executeQuery(query)
        #query = "delete from LHCb_macroscopic_connectivity where author like 'DSONNICK'"
        #self.confDB.executeQuery(query)
        #query = "delete from LHCB_port_properties where author like 'DSONNICK'"
        #self.confDB.executeQuery(query)
        #query = "delete from LHCB_IPINFO where IPNAME NOT like 'pctest'"
        #self.confDB.executeQuery(query)
        self.emit(QtCore.SIGNAL("removealldevicesdone()"))
        print "RemoveAllDevicesWorker.run() end"