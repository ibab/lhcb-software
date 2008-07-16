from PyQt4.QtCore import QThread
from PyQt4 import QtCore

class TFCMunin01Worker(QThread):
    def __init__(self, confDB, db):
        QThread.__init__(self)
        self.confDB = confDB
        self.db = db
    def run(self):
        print "TFCMunin01Worker.run() start"
        self.confDB.connect_masterhugins(self.db)
        self.emit(QtCore.SIGNAL("workFinished()"))
        print "TFCMunin01Worker.run() end"