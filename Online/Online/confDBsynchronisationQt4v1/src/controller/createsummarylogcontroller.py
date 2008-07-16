from controller import Controller
from PyQt4 import QtCore, QtGui
from worker.createsummarylogworker import CreateSummaryLogWorker

class CreateSummaryLogController(Controller):
    def __init__(self, parentController):
        Controller.__init__(self)
        self.parentController = parentController
        self.progressDialog = QtGui.QProgressDialog()
        self.progressDialog.setMinimum(0)
        self.progressDialog.setMaximum(0)
        self.progressDialog.setWindowTitle("Creating summary log")
        self.progressDialog.setLabelText("Creating summary log, please be patient...")
        self.progressDialog.show()
        self.createSummaryLogWorker = CreateSummaryLogWorker(self.parentController.getConfDBCxOracleConnection(), self.parentController.getEquipDBCxOracleConnection())
        self.createSummaryLogWorker.connect(self.createSummaryLogWorker, QtCore.SIGNAL("workProgressed(int)"), self.progressDialog, QtCore.SLOT("setValue(int)"))
        self.createSummaryLogWorker.connect(self.createSummaryLogWorker, QtCore.SIGNAL("workFinished()"), self.onFinish)
        self.createSummaryLogWorker.start()
    def onFinish(self):
        print "InsertNewSparesController.onFinish() start"
        self.progressDialog.hide()
        self.progressDialog.destroy()
        QtGui.QMessageBox.information(None, "Finish", "Creating summary log finished.")
        self.parentController.onRefresh()
        print "InsertNewSparesController.onFinish() start"