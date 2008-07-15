from PyQt4 import QtGui, QtCore

class InsertDevicesProgessDialog(QtGui.QWidget):
    def __init__(self, max = 0, parent = None):
        print "InsertDevicesProgessDialog.__init__() start"
        QtGui.QWidget.__init__(self, parent)
        self.setGeometry(1,1, 400, 300)
        self.center()
        self.setWindowTitle("Inserting new devices")
        self.progressBar = QtGui.QProgressBar(self)
        self.progressBar.setGeometry(10, 200, 380, 30)
        self.progressBar.setMinimum(0)
        self.progressBar.setMaximum(max)
        self.progressBar.show()
        self.infoLabel = QtGui.QLabel("Inserting new devices, please be patient...", self)
        self.infoLabel.setGeometry(10, 10, 200, 30)
        self.show()
        print "InsertDevicesProgessDialog.__init__() end"
    def center(self):
        screen = QtGui.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)