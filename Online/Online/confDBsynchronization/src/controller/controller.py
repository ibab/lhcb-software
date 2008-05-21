import wx
from database.database import ConfigurationDB, EquipmentDB
from database.spare import SpareDB
from view.mainwindow import MainWindow
from config import cDB_name, cDB_user, cDB_pass
from model.devices import Devices
"""
   The controller sets up the mainwindow and keeps track of the model
"""
class Controller(wx.PySimpleApp):
    def OnInit(self):
        #connecting to the databases
        self.confDB = ConfigurationDB()
        self.equipDB = EquipmentDB()
        self.spareDB = SpareDB(self.confDB, self.equipDB)
        #get the statistics of the devices
        self.devices = Devices(self.confDB, self.equipDB)
        self.devices.update()
        #setting up the main window
        self.mainWindow = MainWindow(self)
        self.SetTopWindow(self.mainWindow)
        return True
    def UpdateMainFrameStausPanel(self):
        self.mainWindow.statusPanel.update()
