from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class DeleteSparesWorker(QThread):
    def __init__(self, spareDB, db):
        QThread.__init__(self)
        self.spareDB = spareDB
        self.db = db
    def run(self):
        print "DeleteSparesWorker.run() start"
        self.spareDB.DeleteSparesFromConfDB()
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "DeleteSparesWorker.run() end"