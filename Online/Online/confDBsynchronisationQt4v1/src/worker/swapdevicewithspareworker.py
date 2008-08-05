from PyQt4.QtCore import QThread
from PyQt4 import QtCore, QtGui

class SwapDeviceWithSpareWorker(QThread):
        def __init__(self, devicestring, sparestring, spareDB):
            print "SwapDeviceWithSpareWorker.__init__() start"
            QThread.__init__(self)
            self.spareDB = spareDB
            self.devicestring = devicestring
            self.sparestring = sparestring
            print self.devicestring
            print self.sparestring
            print "SwapDeviceWithSpareWorker.__init__() end"
        def run(self):
            print "SwapDeviceWithSpareWorker.run() start"
            self.emit(QtCore.SIGNAL("workProgressed(int)"), 10)
            sparemacaddress = self.spareDB.equipDB.dhcp.getMACBySerial(self.sparestring)
            if sparemacaddress is None:
                print "macaddress for "+str(self.sparestring)+" not found!"
                self.emit(QtCore.SIGNAL("errorWhileWorking(PyQt_PyObject)"), "macaddress for "+str(self.sparestring)+" not found!")
            else:
                self.emit(QtCore.SIGNAL("workProgressed(int)"), 25)
                self.spareDB.confDB.replaceDeviceBySpare(self.devicestring, self.sparestring, sparemacaddress)
                self.emit(QtCore.SIGNAL("workProgressed(int)"), 50)
                #self.spareDB.equipDB.swapDevices(devicestring, sparestring) #TODO: test this method
                self.emit(QtCore.SIGNAL("workProgressed(int)"), 75)
                self.emit(QtCore.SIGNAL("workFinished()"))
            print "SwapDeviceWithSpareWorker.run() end"