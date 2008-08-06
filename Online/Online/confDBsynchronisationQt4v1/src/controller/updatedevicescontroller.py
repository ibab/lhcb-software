from PyQt4 import QtCore, QtGui
from worker.updatedevicesworker import UpdateDevicesWorker
from controller import Controller

class UpdateDevicesController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.equipDB = self.parentController.getEquipDBCxOracleConnection()
        self.confDB = self.parentController.getConfDBCxOracleConnection()
        self.parentController.connectConfDBPython()
        self.db = self.parentController.getCn()
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(100)
        self.progressDialog.setWindowTitle("Updating devices in ConfDB")
        self.progressDialog.setLabelText("Updating devices in ConfDB, please be patient...")
        self.progressDialog.show()
        self.updateDevicesWorker = UpdateDevicesWorker(self.confDB, self.equipDB, self.db)
        self.updateDevicesWorker.connect(self.updateDevicesWorker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.updateDevicesWorker.connect(self.updateDevicesWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.updateDevicesWorker.start()
    def onFinish(self):
        print "UpdateDevicesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.parentController.devicesActionMutex.unlock()
        QtGui.QMessageBox.information(None, "Finish", "Updating devices finished. Check log-files for details.")
        self.parentController.onRefreshDevices()
        print "UpdateDevicesController.onFinish() start"