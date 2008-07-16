from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class UpdateSparesWorker(QThread):
    def __init__(self, spareDB, db):
        QThread.__init__(self)
        self.spareDB = spareDB
        self.db = db
    def run(self):
        print "InsertNewSparesWorker.run() star"
        self.spareDB.UpdateSpares()
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "InsertNewSparesWorker.run() end"