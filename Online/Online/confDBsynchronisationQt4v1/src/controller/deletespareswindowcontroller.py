from PyQt4 import QtCore, QtGui
from controller import Controller
from view.deletespareswindow import DeleteSparesWindow
from view.deletespareswindow import DevicesList
from deletesparescontroller import DeleteSparesController

class DeleteSparesWindowController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.confDB = self.parentController.getConfDBCxOracleConnection()
        self.equipDB = self.parentController.getEquipDBCxOracleConnection()
        self.newDevicesListModel = []
        allDevices = self.confDB.getAllSparesAsString()
        if len(allDevices) > 0:
            progressDialog = QtGui.QProgressDialog()
            progressDialog.setLabelText("Preparing data...")
            progressDialog.setMinimum(0)
            progressDialog.setMaximum(len(allDevices))
            progressDialog.show()
            i = 0
            for device in allDevices:
                self.newDevicesListModel.append(device)
                i += 1
                progressDialog.setValue(i)
            progressDialog.hide()
            progressDialog.destroy()
        if len(self.newDevicesListModel) == 0:
            QtGui.QMessageBox.information(None, "No spares", "There are no new spares which could be deleted.")
            self.parentController.sparesActionMutex.unlock()
            return
        self.newDevicesWindow = DeleteSparesWindow(self.parentController)
        
        self.selectedDevicesListModel = []
        
        self.selectedNewDevice = None
        self.selectedDevice = None
        
        buttonwidth = 100
        buttonheight = 30
        buttonStartX = 270
        buttonStartY = 80
        buttonDistance = 40
        buttonSeperator = 150
        
        self.addAllButton = QtGui.QPushButton(self.newDevicesWindow)
        self.addAllButton.setText("add all >>")
        self.addAllButton.setToolTip("Adds all spares to the list")
        self.addAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY, buttonwidth, buttonheight))
        
        self.addThisButton = QtGui.QPushButton(self.newDevicesWindow)
        self.addThisButton.setText("add this >>")
        self.addThisButton.setToolTip("Adds the selected spares to the confdb-list")
        self.addThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+buttonDistance, buttonwidth, buttonheight))
        
        self.removeThisButton = QtGui.QPushButton(self.newDevicesWindow)
        self.removeThisButton.setText("<< removes this")
        self.removeThisButton.setToolTip("Removes the selected spares from the list")
        self.removeThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+2*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.removeAllButton = QtGui.QPushButton(self.newDevicesWindow)
        self.removeAllButton.setText("<< remove all")
        self.removeAllButton.setToolTip("Removes all spares from the list")
        self.removeAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+3*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.insertButton = QtGui.QPushButton(self.newDevicesWindow)
        self.insertButton.setText("Delete selected spares ->")
        self.insertButton.setToolTip("Delete all selected spares from Configuration Database")
        self.insertButton.setGeometry(QtCore.QRect(430, 440, 2*buttonwidth, buttonheight))
        
        self.closeButton = QtGui.QPushButton(self.newDevicesWindow)
        self.closeButton.setText("<- Close")
        self.closeButton.setGeometry(QtCore.QRect(10, 440, 2*buttonwidth, buttonheight))
        self.closeButton.setToolTip("Closes this window without any change to the database")
        
        self.newDevicesList = DevicesList(self.newDevicesWindow, QtCore.QRect(10,30,250,400), "Spares in ConfDB")
        self.newDevicesList.setDevices(self.newDevicesListModel)
        self.newDevicesList.setToolTip("This is the list with the spares in ConfDB.")
        
        self.selectedDevicesList = DevicesList(self.newDevicesWindow, QtCore.QRect(380,30,250,400), "Spares to be deleted")
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.selectedDevicesList.setToolTip("This is the list with the devices which\nwill be inserted into configuration database")
        
        self.newDevicesWindow.connect(self.addAllButton, QtCore.SIGNAL("clicked()"), self.onAddAll)
        self.newDevicesWindow.connect(self.addThisButton, QtCore.SIGNAL("clicked()"), self.onAddThis)
        self.newDevicesWindow.connect(self.removeAllButton, QtCore.SIGNAL("clicked()"), self.onRemoveAll)
        self.newDevicesWindow.connect(self.removeThisButton, QtCore.SIGNAL("clicked()"), self.onRemoveThis)
        
        self.newDevicesWindow.connect(self.insertButton, QtCore.SIGNAL("clicked()"), self.onDelete)
        self.newDevicesWindow.connect(self.closeButton, QtCore.SIGNAL("clicked()"), self.newDevicesWindow.close)
        
        #self.newDevicesWindow.connect(self.newDevicesList, QtCore.SIGNAL('itemClicked(QTreeWidgetItem*, int)'), self.onClickNewDevices)
        #self.newDevicesWindow.connect(self.selectedDevicesList, QtCore.SIGNAL('itemClicked(QTreeWidgetItem*, int)'), self.onClickSelectedDevices)
        
        self.newDevicesWindow.show()
    def setNewDevicesListModel(self, newDevicesListModel):
        self.newDevicesListModel = newDevicesListModel
    def onAddAll(self):
        print "DeleteSparesWindowController.onAddAll() start"
        for item in self.newDevicesListModel:
            self.selectedDevicesListModel.append(item)
        self.newDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteSparesWindowController.onAddAll() end"
    def onAddThis(self):
        print "DeleteSparesWindowController.onAddThis() start"
        for item in self.newDevicesList.selectedItems():
            if item is not None:
                #print item.data(0, 32).toString()
                item = str(item.text(0))
                self.newDevicesListModel.remove(item)
                self.selectedDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteSparesWindowController.onAddThis() end"
    def onRemoveAll(self):
        print "DeleteSparesWindowController.onRemoveAll() start"
        for item in self.selectedDevicesListModel:
            self.newDevicesListModel.append(item)
        self.selectedDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteSparesWindowController.onRemoveAll() end"
    def onRemoveThis(self):
        print "DeleteSparesWindowController.onRemoveThis() start"
        for item in self.selectedDevicesList.selectedItems():
            if item is not None:
                item = str(item.text(0))
                self.selectedDevicesListModel.remove(item)
                self.newDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteSparesWindowController.onRemoveThis() end"
    def onDelete(self):
        print "DeleteSparesWindowController.onDelete() start"
        if len(self.selectedDevicesListModel) == 0:
            print "No spares selected!"
        else:
            itemSerials = []
            for item in self.selectedDevicesListModel:
                itemSerials.append(str(item).split(" (")[1][:-1])
            self.newDevicesWindow.hide()
            self.newDevicesWindow.destroy()
            self.deleteSparesController = DeleteSparesController(itemSerials, self.parentController)
        print "DeleteSparesWindowController.onInsert() end"
    def onClickNewDevices(self):
        print "DeleteSparesWindowController.onClickNewDevices() start"
        data = self.newDevicesList.currentItem().data(0, 32).toString()
        if not self.newDevicesList.currentItem().isSelected():
            self.selectedNewDevice = None
            return
        if data is not None:
            self.selectedNewDevice = str(data)
            print "Click on "+self.selectedNewDevice
        else:
            self.selectedNewDevice = None
            print "no item selected"
        print "DeleteSparesWindowController.onClickNewDevices() end"
    def onClickSelectedDevices(self):
        print "DeleteSparesWindowController.onClickSelectedDevices() start"
        data = self.selectedDevicesList.currentItem().data(0, 32).toString()
        if data is not None:
            self.selectedDevice = str(data)
            print "Click on "+self.selectedDevice
        else:
            self.selectedDevice = None
            print "no item selected"
        print "DeleteSparesWindowController.onClickSelectedDevices() end"
