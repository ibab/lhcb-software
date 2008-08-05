from PyQt4 import QtCore, QtGui
from controller import Controller
from view.deletedeviceswindow import DeleteDevicesWindow
from view.deletedeviceswindow import DevicesList
from deletedevicescontroller import DeleteDevicesController

class DeleteDevicesWindowController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.confDB = self.parentController.getConfDBCxOracleConnection()
        self.equipDB = self.parentController.getEquipDBCxOracleConnection()
        self.newDevicesListModel = []
        allDevices = self.confDB.getAllDevicesAsString()
        if len(allDevices) == 0:
            QtGui.QMessageBox.information(None, "No devices", "At the moment there exist no devices\nin ConfDB which could be deleted!.")
            return
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
            QtGui.QMessageBox.information(None, "No devices", "At the moment there exist no devices\nin ConfDB which could be deleted!.")
            return
        self.deleteDevicesWindow = DeleteDevicesWindow()
        
        self.selectedDevicesListModel = []
        
        self.selectedNewDevice = None
        self.selectedDevice = None
        
        buttonwidth = 100
        buttonheight = 30
        buttonStartX = 270
        buttonStartY = 80
        buttonDistance = 40
        buttonSeperator = 150
        
        self.addAllButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.addAllButton.setText("add all >>")
        self.addAllButton.setToolTip("Adds all devices to the confdb-list")
        self.addAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY, buttonwidth, buttonheight))
        
        self.addThisButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.addThisButton.setText("add this >>")
        self.addThisButton.setToolTip("Adds the selected device to the confdb-list")
        self.addThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+buttonDistance, buttonwidth, buttonheight))
        
        self.removeThisButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.removeThisButton.setText("<< removes this")
        self.removeThisButton.setToolTip("Removes the selected device from the confdb-list")
        self.removeThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+2*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.removeAllButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.removeAllButton.setText("<< remove all")
        self.removeAllButton.setToolTip("Removes all devices from the confdb-list")
        self.removeAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+3*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.deleteButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.deleteButton.setText("Delete selected devices ->")
        self.deleteButton.setToolTip("Delete all selected devices from Configuration Database")
        self.deleteButton.setGeometry(QtCore.QRect(430, 440, 2*buttonwidth, buttonheight))
        
        self.closeButton = QtGui.QPushButton(self.deleteDevicesWindow)
        self.closeButton.setText("<- Close")
        self.closeButton.setGeometry(QtCore.QRect(10, 440, 2*buttonwidth, buttonheight))
        self.closeButton.setToolTip("Closes this window without any change to the database")
        
        self.newDevicesList = DevicesList(self.deleteDevicesWindow, QtCore.QRect(10,30,250,400), "Devices in ConfDB")
        self.newDevicesList.setDevices(self.newDevicesListModel)
        self.newDevicesList.setToolTip("This is the list with all devices in ConfDB.")
        
        self.selectedDevicesList = DevicesList(self.deleteDevicesWindow, QtCore.QRect(380,30,250,400), "Devices to delete from confdb")
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.selectedDevicesList.setToolTip("This is the list with the devices which\nwill be deleted from configuration database")
        
        self.deleteDevicesWindow.connect(self.addAllButton, QtCore.SIGNAL("clicked()"), self.onAddAll)
        self.deleteDevicesWindow.connect(self.addThisButton, QtCore.SIGNAL("clicked()"), self.onAddThis)
        self.deleteDevicesWindow.connect(self.removeAllButton, QtCore.SIGNAL("clicked()"), self.onRemoveAll)
        self.deleteDevicesWindow.connect(self.removeThisButton, QtCore.SIGNAL("clicked()"), self.onRemoveThis)
        
        self.deleteDevicesWindow.connect(self.deleteButton, QtCore.SIGNAL("clicked()"), self.onDelete)
        self.deleteDevicesWindow.connect(self.closeButton, QtCore.SIGNAL("clicked()"), self.onClose)

        self.deleteDevicesWindow.show()
    def setNewDevicesListModel(self, newDevicesListModel):
        self.newDevicesListModel = newDevicesListModel
    def onAddAll(self):
        print "DeleteDevicesWindowController.onAddAll() start"
        for item in self.newDevicesListModel:
            self.selectedDevicesListModel.append(item)
        self.newDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteDevicesWindowController.onAddAll() end"
    def onAddThis(self):
        print "DeleteDevicesWindowController.onAddThis() start"
        for item in self.newDevicesList.selectedItems():
            if item is not None:
                #print item.data(0, 32).toString()
                item = str(item.text(0))
                self.newDevicesListModel.remove(item)
                self.selectedDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteDevicesWindowController.onAddThis() end"
    def onRemoveAll(self):
        print "DeleteDevicesWindowController.onRemoveAll() start"
        for item in self.selectedDevicesListModel:
            self.newDevicesListModel.append(item)
        self.selectedDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteDevicesWindowController.onRemoveAll() end"
    def onRemoveThis(self):
        print "DeleteDevicesWindowController.onRemoveThis() start"
        for item in self.selectedDevicesList.selectedItems():
            if item is not None:
                item = str(item.text(0))
                self.selectedDevicesListModel.remove(item)
                self.newDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "DeleteDevicesWindowController.onRemoveThis() end"
    def onDelete(self):
        print "DeleteDevicesWindowController.onDelete() start"
        if len(self.selectedDevicesListModel) == 0:
            print "No devices to insert selected!"
        else:
            itemSerials = []
            for item in self.selectedDevicesListModel:
                itemSerials.append(str(item).split(" (")[0])
            self.deleteDevicesWindow.hide()
            self.deleteDevicesWindow.destroy()
            self.deleteDevicesController = DeleteDevicesController(self.parentController, itemSerials)
        print "DeleteDevicesWindowController.onDelete() end"
    def onClose(self):
        print "DeleteDevicesWindowController.onClose() start"
        self.deleteDevicesWindow.hide()
        self.deleteDevicesWindow.destroy()
        print "DeleteDevicesWindowController.onClose() end"
    def onClickNewDevices(self):
        print "DeleteDevicesWindowController.onClickNewDevices() start"
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
        print "DeleteDevicesWindowController.onClickNewDevices() end"
    def onClickSelectedDevices(self):
        print "DeleteDevicesWindowController.onClickSelectedDevices() start"
        data = self.selectedDevicesList.currentItem().data(0, 32).toString()
        if data is not None:
            self.selectedDevice = str(data)
            print "Click on "+self.selectedDevice
        else:
            self.selectedDevice = None
            print "no item selected"
        print "DeleteDevicesWindowController.onClickSelectedDevices() end"
