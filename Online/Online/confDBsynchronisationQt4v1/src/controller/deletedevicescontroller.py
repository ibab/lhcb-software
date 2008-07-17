from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import QObject
from controller import Controller
from PyQt4.QtGui import QDialog
from worker.removealldevicesworker import RemoveAllDevicesWorker

class DeleteDevicesController(Controller, QObject):
    def __init__(self, parentController):
        print "DeleteDevicesController.__init__() start"
        Controller.__init__(self)
        QObject.__init__(self)
        self.parentController = parentController
        self.db = self.parentController.getCn()
        print str(self.db)
        reply = QtGui.QMessageBox.question(None, "Confirmation",
                                           "Do you really want to delete all devices from Configuration Database?", QtGui.QMessageBox.Yes,
                                           QtGui.QMessageBox.No)
        if reply == QtGui.QMessageBox.Yes:
            count = self.parentController.getConfDBCxOracleConnection().getDevicesCount()
            self.progressDialog = QtGui.QProgressDialog()
            self.progressDialog.setLabelText("Deleting devices...")
            self.progressDialog.setMinimum(0)
            self.progressDialog.setMaximum(count)
            self.removeAllDevicesWorker = RemoveAllDevicesWorker(self.parentController.getConfDBCxOracleConnection(), self.db, count)
            self.connect(self.removeAllDevicesWorker, QtCore.SIGNAL("setValue(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
            self.connect(self.removeAllDevicesWorker, QtCore.SIGNAL("removealldevicesdone()"), self.onFinish)
            self.progressDialog.show()
            self.removeAllDevicesWorker.start()
        print "DeleteDevicesController.__init__() end"
    def onFinish(self):
        print "DeleteDevicesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.disconnectConfDBPython()
        QtGui.QMessageBox.information(None, "Finish", "Deleteing devices from ConfDB has finished. Check log-files for details.")
        if self.parentController is not None:
            self.parentController.onRefresh()
        else:
            print "self.parentController is None "+str(self.parentController)
        print "DeleteDevicesController.onFinish() end"
