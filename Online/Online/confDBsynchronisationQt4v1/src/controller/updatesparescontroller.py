from PyQt4 import QtCore, QtGui
from controller import Controller
from worker.updatesparesworker import UpdateSparesWorker

class UpdateNewSparesController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.spareDB = self.parentController.getSpareDB()
        self.db = self.parentController.getCn()
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(0)
        self.progressDialog.setWindowTitle("Updating spares in ConfDB")
        self.progressDialog.setLabelText("Updating in ConfDB, please be patient...")
        self.progressDialog.show()
        self.updateSparesWorker = UpdateSparesWorker(self.spareDB, self.db)
        self.updateSparesWorker.connect(self.updateSparesWorker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.updateSparesWorker.connect(self.updateSparesWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.updateSparesWorker.start()
    def onFinish(self):
        print "InsertNewSparesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        QtGui.QMessageBox.information(None, "Finish", "Updating spares finished. Check log-files for details.")
        self.parentController.onRefreshSpares()
        print "InsertNewSparesController.onFinish() start"