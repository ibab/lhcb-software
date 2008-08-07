from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class DeleteSparesWorker(QThread):
    def __init__(self, itemSerials, spareDB, db):
        QThread.__init__(self)
        self.itemSerials = itemSerials
        self.spareDB = spareDB
        self.db = db
    def run(self):
        print "DeleteSparesWorker.run() start"
        self.spareDB.DeleteSparesBySerials(self.itemSerials, self.db)
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "DeleteSparesWorker.run() end"