from PyQt4 import QtCore, QtGui
from controller import Controller
from worker.insertnewsparesworker import InsertNewSparesWorker

class InsertNewSparesController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(0)
        self.progressDialog.setWindowTitle("Inserting new spares in ConfDB")
        self.progressDialog.setLabelText("Inserting new spares in ConfDB, please be patient...")
        self.progressDialog.show()
        self.insertNewSparesWorker = InsertNewSparesWorker(self.parentController.getSpareDB(), self.parentController.getCn())
        self.insertNewSparesWorker.connect(self.insertNewSparesWorker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.insertNewSparesWorker.connect(self.insertNewSparesWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.insertNewSparesWorker.start()
    def onFinish(self):
        print "InsertNewSparesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        QtGui.QMessageBox.information(None, "Finish", "Inserting spares finished. Check log-files for details.")
        self.parentController.onRefreshSpares()
        print "InsertNewSparesController.onFinish() start"