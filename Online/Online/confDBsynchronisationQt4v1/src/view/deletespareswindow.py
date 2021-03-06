from PyQt4 import QtGui, QtCore, Qt
from PyQt4.QtGui import QFont


class DeleteSparesWindow(QtGui.QWidget):
    def __init__(self, parentController):
        self.parentController = parentController
        QtGui.QWidget.__init__(self)
        self.resize(640, 480)
        self.center()
        #self.show()
    def center(self):
        screen = QtGui.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)
    def closeEvent(self, event):
        print "DeleteDevicesWindow.closeEvent() start"
        self.parentController.sparesActionMutex.unlock()
        event.accept()
        print "DeleteDevicesWindow.closeEvent() end"

class DevicesList(QtGui.QTreeWidget):
    def __init__(self, parent, geometry, headerLabel):
        print "DevicesList.__init__() start"
        QtGui.QTreeWidget.__init__(self, parent)
        self.parent = parent
        self.setGeometry(geometry)
        self.setFont( QFont("Arial", 10, QFont.Normal) )
        self.setHeaderLabel(headerLabel)
        self.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        print "DevicesList.__init__() end"
    def setDevices(self, items):
        print "DevicesList.setDevices() start"
        self.clear()
        self.items = items
        for item in self.items:
            sn = str(item).split(" (")[1][:-1]
            newItem = QtGui.QTreeWidgetItem(self)
            newItem.setText(0, str(item))
            newItem.setFont(0, QFont("Arial", 10, QFont.Bold) )
            newItem.setData(0, 32, QtCore.QVariant( str(sn) ))
        self.sortItems(0, QtCore.Qt.AscendingOrder)
        print "DevicesList.setDevices() end"