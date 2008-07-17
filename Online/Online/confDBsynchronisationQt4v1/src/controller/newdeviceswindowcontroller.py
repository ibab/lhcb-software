from PyQt4 import QtCore, QtGui
from controller import Controller
from view.newdeviceswindow import NewDevicesWindow
from view.newdeviceswindow import DevicesList
from insertnewdevicescontroller import InsertNewDevicesController

class NewDevicesWindowController(Controller):
    def __init__(self, confDB, equipDB, parentController):
        Controller.__init__(self)
        self.parentController = parentController
        self.confDB = confDB
        self.equipDB = equipDB
        system = self.equipDB.getSystem()
        self.newDevicesListModel = []
        allDevices = system.getAllDevices()
        if len(allDevices) > 0:
            progressDialog = QtGui.QProgressDialog()
            progressDialog.setLabelText("Preparing data...")
            progressDialog.setMinimum(0)
            progressDialog.setMaximum(len(allDevices))
            progressDialog.show()
        i = 0
        for device in system.getAllDevices():
            if self.confDB.getDeviceBySerial(device.sn) is None:
                self.newDevicesListModel.append(device.devicename+" ("+device.sn+")")
            i += 1
            progressDialog.setValue(i)
        progressDialog.hide()
        progressDialog.destroy()
        if len(self.newDevicesListModel) == 0:
            QtGui.QMessageBox.information(None, "No new devices", "There are no new devices.\nIt seems that configuration database is synchronal with lhcbintegration.")
        self.newDevicesWindow = NewDevicesWindow()
        
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
        self.addAllButton.setToolTip("Adds all devices to the confdb-list")
        self.addAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY, buttonwidth, buttonheight))
        
        self.addThisButton = QtGui.QPushButton(self.newDevicesWindow)
        self.addThisButton.setText("add this >>")
        self.addThisButton.setToolTip("Adds the selected device to the confdb-list")
        self.addThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+buttonDistance, buttonwidth, buttonheight))
        
        self.removeThisButton = QtGui.QPushButton(self.newDevicesWindow)
        self.removeThisButton.setText("<< removes this")
        self.removeThisButton.setToolTip("Removes the selected device from the confdb-list")
        self.removeThisButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+2*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.removeAllButton = QtGui.QPushButton(self.newDevicesWindow)
        self.removeAllButton.setText("<< remove all")
        self.removeAllButton.setToolTip("Removes all devices from the confdb-list")
        self.removeAllButton.setGeometry(QtCore.QRect(buttonStartX, buttonStartY+3*buttonDistance+buttonSeperator, buttonwidth, buttonheight))
        
        self.insertButton = QtGui.QPushButton(self.newDevicesWindow)
        self.insertButton.setText("Insert selected new devices ->")
        self.insertButton.setToolTip("Insert all selected new devices into Configuration Database")
        self.insertButton.setGeometry(QtCore.QRect(430, 440, 2*buttonwidth, buttonheight))
        
        self.closeButton = QtGui.QPushButton(self.newDevicesWindow)
        self.closeButton.setText("<- Close")
        self.closeButton.setGeometry(QtCore.QRect(10, 440, 2*buttonwidth, buttonheight))
        self.closeButton.setToolTip("Closes this window without any change to the database")
        
        self.label = QtGui.QLabel(self.newDevicesWindow)
        self.label.setText("Test")
        self.label.setGeometry(QtCore.QRect(50,50,200,200))
        
        self.newDevicesList = DevicesList(self.newDevicesWindow, QtCore.QRect(10,30,250,400), "New devices")
        self.newDevicesList.setDevices(self.newDevicesListModel)
        self.newDevicesList.setToolTip("This is the list with devices from lhcbintegration\nand which do not exist in the configuration database yet.")
        
        self.selectedDevicesList = DevicesList(self.newDevicesWindow, QtCore.QRect(380,30,250,400), "Devices to insert into confdb")
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.selectedDevicesList.setToolTip("This is the list with the devices which\nwill be inserted into configuration database")
        
        self.newDevicesWindow.connect(self.addAllButton, QtCore.SIGNAL("clicked()"), self.onAddAll)
        self.newDevicesWindow.connect(self.addThisButton, QtCore.SIGNAL("clicked()"), self.onAddThis)
        self.newDevicesWindow.connect(self.removeAllButton, QtCore.SIGNAL("clicked()"), self.onRemoveAll)
        self.newDevicesWindow.connect(self.removeThisButton, QtCore.SIGNAL("clicked()"), self.onRemoveThis)
        
        self.newDevicesWindow.connect(self.insertButton, QtCore.SIGNAL("clicked()"), self.onInsert)
        self.newDevicesWindow.connect(self.closeButton, QtCore.SIGNAL("clicked()"), self.onClose)
        
        #self.newDevicesWindow.connect(self.newDevicesList, QtCore.SIGNAL('itemClicked(QTreeWidgetItem*, int)'), self.onClickNewDevices)
        #self.newDevicesWindow.connect(self.selectedDevicesList, QtCore.SIGNAL('itemClicked(QTreeWidgetItem*, int)'), self.onClickSelectedDevices)
        
        self.newDevicesWindow.show()
    def setNewDevicesListModel(self, newDevicesListModel):
        self.newDevicesListModel = newDevicesListModel
    def onAddAll(self):
        print "NewDevicesWindowController.onAddAll() start"
        for item in self.newDevicesListModel:
            self.selectedDevicesListModel.append(item)
        self.newDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "NewDevicesWindowController.onAddAll() end"
    def onAddThis(self):
        print "NewDevicesWindowController.onAddThis() start"
        for item in self.newDevicesList.selectedItems():
            if item is not None:
                #print item.data(0, 32).toString()
                item = str(item.text(0))
                self.newDevicesListModel.remove(item)
                self.selectedDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "NewDevicesWindowController.onAddThis() end"
    def onRemoveAll(self):
        print "NewDevicesWindowController.onRemoveAll() start"
        for item in self.selectedDevicesListModel:
            self.newDevicesListModel.append(item)
        self.selectedDevicesListModel = []
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "NewDevicesWindowController.onRemoveAll() end"
    def onRemoveThis(self):
        print "NewDevicesWindowController.onRemoveThis() start"
        for item in self.selectedDevicesList.selectedItems():
            if item is not None:
                item = str(item.text(0))
                self.selectedDevicesListModel.remove(item)
                self.newDevicesListModel.append(item)
        self.selectedDevicesList.setDevices(self.selectedDevicesListModel)
        self.newDevicesList.setDevices(self.newDevicesListModel)
        print "NewDevicesWindowController.onRemoveThis() end"
    def onInsert(self):
        print "NewDevicesWindowController.onInsert() start"
        if len(self.selectedDevicesListModel) == 0:
            print "No devices to insert selected!"
        else:
            itemSerials = []
            for item in self.selectedDevicesListModel:
                itemSerials.append(str(item).split(" (")[1][:-1])
            self.newDevicesWindow.hide()
            self.newDevicesWindow.destroy()
            self.insertNewDevicesController = InsertNewDevicesController(itemSerials, self.parentController)
        print "NewDevicesWindowController.onInsert() end"
    def onClose(self):
        print "NewDevicesWindowController.onClose() start"
        self.newDevicesWindow.hide()
        self.newDevicesWindow.destroy()
        print "NewDevicesWindowController.onClose() end"
    def onClickNewDevices(self):
        print "NewDevicesWindowController.onClickNewDevices() start"
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
        print "NewDevicesWindowController.onClickNewDevices() end"
    def onClickSelectedDevices(self):
        print "NewDevicesWindowController.onClickSelectedDevices() start"
        data = self.selectedDevicesList.currentItem().data(0, 32).toString()
        if data is not None:
            self.selectedDevice = str(data)
            print "Click on "+self.selectedDevice
        else:
            self.selectedDevice = None
            print "no item selected"
        print "NewDevicesWindowController.onClickSelectedDevices() end"
