from PyQt4 import QtCore, QtGui
from controller import Controller
from worker.tfcmunin01worker import TFCMunin01Worker

class TFCMunin01Controller(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(0)
        self.progressDialog.setWindowTitle("Connecting Hugins to TFCMUNIN01 in ConfDB")
        self.progressDialog.setLabelText("Connecting Hugins to TFCMUNIN01, please be patient...")
        self.progressDialog.show()
        self.TFCMunin01Worker = TFCMunin01Worker(self.parentController.getConfDBCxOracleConnection(), self.parentController.getCn())
        self.TFCMunin01Worker.connect(self.TFCMunin01Worker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.TFCMunin01Worker.connect(self.TFCMunin01Worker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.TFCMunin01Worker.start()
    def onFinish(self):
        print "InsertNewSparesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        self.parentController.devicesActionMutex.unlock()
        QtGui.QMessageBox.information(None, "Finish", "Connecting Hugins to TFCMUNIN01. Check log-files for details.")
        print "InsertNewSparesController.onFinish() start"