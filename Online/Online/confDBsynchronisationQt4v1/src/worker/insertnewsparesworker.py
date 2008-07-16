from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class InsertNewSparesWorker(QThread):
    def __init__(self, spareDB, db):
        QThread.__init__(self)
        self.spareDB = spareDB
        self.db = db
    def run(self):
        print "InsertNewSparesWorker.run() start"
        new_spares = self.spareDB.getNewSpares()
        if new_spares is not None:
            print "found "+str(len(new_spares))+" new spares"
        for spare in new_spares:
            self.spareDB.logfile.write("inserting spare "+str(spare)+" into confdb")
            try:
                spare.insert(self.db)
            except RuntimeError, inst:
                self.spareDB.logfile.write("error inserting spare "+str(spare)+" into confdb.\nReason: "+str(inst))
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "InsertNewSparesWorker.run() start"