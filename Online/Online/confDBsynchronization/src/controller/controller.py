import wx
from database.database import ConfigurationDB, EquipmentDB
from database.spare import SpareDB
from view.mainwindow import MainWindow
from config import cDB_name, cDB_user, cDB_pass
#from model.devices import Devices
from devices import Devices
from model.device import Device
"""
   The controller sets up the mainwindow, keeps track of the model and
   updates the view if necessary
"""
class Controller(wx.PySimpleApp):
    """
        builing up the connections to the database
    """
    def OnInit(self):
        #connecting to the databases
        self.confDB = ConfigurationDB()
        self.equipDB = EquipmentDB()
        self.spareDB = SpareDB(self.confDB, self.equipDB)
        #get the statistics of the devices
        self.devices = Devices(self)
        self.devices.update()
        #setting up the main window
        self.mainWindow = MainWindow(self)
        self.SetTopWindow(self.mainWindow)
        return True
    def UpdateMainFrameStatusPanel(self):
        wx.CallAfter(self.mainWindow.statusPanel.update)
    def UpdateMainFrameSparePanel(self):
        wx.CallAfter(self.mainWindow.sparePanel.update)
    def UpdateMainFrame(self):
        wx.CallAfter(self.UpdateMainFrameStatusPanel)
        wx.CallAfter(self.UpdateMainFrameSparePanel)
