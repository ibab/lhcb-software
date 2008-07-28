from worker.insertdevicesworker import InsertDevicesWorker
from controller import Controller
from PyQt4 import QtCore, QtGui

class InsertNewDevicesController(object):
    def __init__(self, serialnbs, parentController):
        print "InsertNewDevicesController.__init__() start"
        self.parentController = parentController
        self.serialnbs = serialnbs
        max = len(serialnbs)
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setLabelText("Inserting new devices...")
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(max)
        self.insertDevicesWorker = InsertDevicesWorker(serialnbs, self.parentController.getEquipDBCxOracleConnection(), self.parentController.getConfDBCxOracleConnection(), self.parentController.getCn())
        self.progressDialog.connect(self.insertDevicesWorker, QtCore.SIGNAL("workProgressed(int)"), QtCore.SLOT("setValue(int)"))
        self.progressDialog.connect(self.insertDevicesWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.progressDialog.show()
        self.insertDevicesWorker.start()
        print "InsertNewDevicesController.__init()__ end"
    def onFinish(self):
        print "InsertNewDevicesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.parentController.onRefresh()
        QtGui.QMessageBox.information(None, "Finish", "Inserting devices finished. Check log-files for details.")
        print "InsertNewDevicesController.onFinish() end"