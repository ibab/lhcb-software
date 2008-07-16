from PyQt4 import QtCore, QtGui
from controller import Controller
from view.swapdeviceswindow import SwapDevicesWindow, DevicesList

class SwapDevicesWindowController(Controller):
    def __init__(self, parentController):
        Controller.__init__(self)
        self.parentController = parentController
        self.equipDB = self.parentController.getEquipDBCxOracleConnection()
        self.confDB = self.parentController.getConfDBCxOracleConnection()
        self.parentController.connectConfDBPython()
        self.db = self.parentController.getCn()
        self.swapDevicesWindow = SwapDevicesWindow()
        self.spareListModel = self.confDB.getAllDevicesAsString()
        self.devicesListModel = self.confDB.getAllSparesAsString()
        self.allModel = []
        for item in self.spareListModel:
            self.allModel.append(item)
        for item in self.devicesListModel:
            self.allModel.append(item)
        self.devices1List = DevicesList(self.swapDevicesWindow, QtCore.QRect(10,30,250,400), "Device 1")
        self.devices1List.setDevices(self.allModel)
        self.devices1List.setToolTip("Here you select device 1 for the swap")
        self.devices2List = DevicesList(self.swapDevicesWindow, QtCore.QRect(280,30,250,400), "Device 2")
        self.devices2List.setDevices(self.allModel)
        self.devices2List.setToolTip("Here you select device 2 for the swap")
        
        buttonwidth = 80
        buttonheight = 30
        
        self.swapButton = QtGui.QPushButton(self.swapDevicesWindow)
        self.swapButton.setText("Swap")
        self.swapButton.setToolTip("Swap the two selected devices")
        self.swapButton.setGeometry(QtCore.QRect(550, 30, buttonwidth, buttonheight))
        
        self.closeButton = QtGui.QPushButton(self.swapDevicesWindow)
        self.closeButton.setText("Close")
        self.closeButton.setToolTip("Close the window and don't make any\Changes in any database")
        self.closeButton.setGeometry(QtCore.QRect(550, 380, buttonwidth, buttonheight))
        
        self.swapDevicesWindow.connect(self.swapButton, QtCore.SIGNAL("clicked()"), self.onSwap)
        self.swapDevicesWindow.connect(self.closeButton, QtCore.SIGNAL("clicked()"), self.onClose)
        self.swapDevicesWindow.show()
    def onSwap(self):
        print "SwapDevicesWindowController.onSwap() start"
        self.device1 = None
        self.device2 = None
        if self.devices1List.currentItem() is not None:
            self.device1 = self.devices1List.currentItem().data(0, 32).toString()
        if self.devices2List.currentItem() is not None:
            self.device2 = self.devices2List.currentItem().data(0, 32).toString()
        if self.device1 is None:
            print "no device 1 selected"
            return
        if self.device2 is None:
            print "no device 2 selected"
            return
        if str(self.device1) == str(self.device2):
            print "cannot swap 2 identical devices"
            return
        print "device1: "+str(self.device1)
        print "device2: "+str(self.device2)
        print "SwapDevicesWindowController.onSwap() end"
    def onClose(self):
        print "SwapDevicesWindowController.onClose() start"
        self.swapDevicesWindow.hide()
        self.swapDevicesWindow.destroy()
        print "SwapDevicesWindowController.onClose() end"