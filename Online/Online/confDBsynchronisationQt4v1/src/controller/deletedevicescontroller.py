from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import QObject
from controller import Controller
from PyQt4.QtGui import QDialog
from worker.removealldevicesworker import RemoveAllDevicesWorker

class DeleteDevicesController(object):
    def __init__(self, parentController, itemSerials):
        print "DeleteDevicesController.__init__() start"
        self.parentController = parentController
        self.itemSerials = itemSerials
        self.db = self.parentController.getCn()
        print str(self.db)
        reply = QtGui.QMessageBox.question(None, "Confirmation",
                                           "Do you really want to delete all selected devices from Configuration Database?", QtGui.QMessageBox.Yes,
                                           QtGui.QMessageBox.No)
        if reply == QtGui.QMessageBox.Yes:
            count = self.parentController.getConfDBCxOracleConnection().getDevicesCount()
            self.progressDialog = QtGui.QProgressDialog()
            self.progressDialog.setLabelText("Deleting devices...")
            self.progressDialog.setMinimum(0)
            self.progressDialog.setMaximum(count)
            self.removeAllDevicesWorker = RemoveAllDevicesWorker(self.parentController.getConfDBCxOracleConnection(), self.db, count, self.itemSerials)
            self.removeAllDevicesWorker.connect(self.removeAllDevicesWorker, QtCore.SIGNAL("setValue(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
            self.removeAllDevicesWorker.connect(self.removeAllDevicesWorker, QtCore.SIGNAL("removealldevicesdone()"), self.onFinish)
            self.progressDialog.show()
            self.removeAllDevicesWorker.start()
        print "DeleteDevicesController.__init__() end"
    def onFinish(self):
        print "DeleteDevicesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.parentController.devicesActionMutex.unlock()
        QtGui.QMessageBox.information(None, "Finish", "Deleteing devices from ConfDB has finished. Check log-files for details.")
        self.parentController.onRefreshDevices()
        print "DeleteDevicesController.onFinish() end"
