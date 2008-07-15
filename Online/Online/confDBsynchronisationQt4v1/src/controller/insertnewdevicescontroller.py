from worker.insertdevicesworker import InsertDevicesWorker
from controller import Controller
from view.insertdevicesprogresswindow import InsertDevicesProgessDialog
from PyQt4 import QtCore, QtGui

class InsertNewDevicesController(Controller):
    def __init__(self, serialnbs, parentController):
        print "InsertNewDevicesController.__init__() start"
        Controller.__init__(self)
        self.parentController = parentController
        self.serialnbs = serialnbs
        self.equipDB = self.getEquipDBCxOracleConnection()
        self.confDB = self.getConfDBCxOracleConnection()
        self.connectConfDBPython()
        self.db = self.getCn()
        if self.db is None or self.db == False:
            print "error getting connection"
            return
        print "These serials will be inserted:"
        for serial in serialnbs:
            print serial
        self.workDone = 0
        max = len(serialnbs)
        self.insertDevicesProgessDialog = InsertDevicesProgessDialog(max)
        self.insertDevicesWorker = InsertDevicesWorker(serialnbs, self.equipDB, self.confDB, self.db)
        self.insertDevicesProgessDialog.connect(self.insertDevicesWorker, QtCore.SIGNAL("progress"), self.onProgress)
        self.insertDevicesProgessDialog.connect(self.insertDevicesWorker, QtCore.SIGNAL("insertdevicesfinish"), self.onFinish)
        self.insertDevicesWorker.start()
        print "InsertNewDevicesController.__init()__ end"
    def onProgress(self):
        print "InsertNewDevicesController.onProgress() start"
        self.workDone += 1
        self.insertDevicesProgessDialog.progressBar.setValue(self.workDone)
        print "InsertNewDevicesController.onProgress() end"
    def onFinish(self):
        print "InsertNewDevicesController.onFinish() start"
        self.disconnectConfDBPython()
        self.insertDevicesProgessDialog.hide()
        self.insertDevicesProgessDialog.destroy()
        QtGui.QMessageBox.information(None, "Finish", "Inserting devices finished. Check log-files for details.")
        self.parentController.onRefresh()
        print "InsertNewDevicesController.onFinish() end"