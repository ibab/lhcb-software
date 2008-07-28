from PyQt4 import QtCore, QtGui
from controller import Controller
from worker.deletesparesworker import DeleteSparesWorker

class DeleteSparesController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.spareDB = self.parentController.getSpareDB()
        self.db = self.parentController.getCn()
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(0)
        self.progressDialog.setWindowTitle("Deleting spares from ConfDB")
        self.progressDialog.setLabelText("Deleting spares from ConfDB, please be patient...")
        self.progressDialog.show()
        self.deleteSparesWorker = DeleteSparesWorker(self.spareDB, self.db)
        self.deleteSparesWorker.connect(self.deleteSparesWorker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.deleteSparesWorker.connect(self.deleteSparesWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.deleteSparesWorker.start()
    def onFinish(self):
        print "UpdateDevicesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.parentController.onRefresh()
        QtGui.QMessageBox.information(None, "Finish", "Deleting spares finished. Check log-files for details.")
        print "UpdateDevicesController.onFinish() start"