import wx
from database.database import ConfigurationDB, EquipmentDB
from database.spare import SpareDB
from view.mainwindow import MainWindow
from config import cDB_name, cDB_user, cDB_pass
#from model.devices import Devices
from devices import Devices
from model.device import Device
from time import strftime
from string import replace
from string import upper, lower, split
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
        self.logfile = self.createSessionLog()
        self.confDB = ConfigurationDB(self.logfile)
        self.equipDB = EquipmentDB(self.logfile)
        self.spareDB = SpareDB(self.confDB, self.equipDB, self.logfile)
        #get the statistics of the devices
        self.collectData()
        #self.devices = Devices(self)
        #self.devices.update()
        #setting up the main window
        self.mainWindow = MainWindow(self)
        self.SetTopWindow(self.mainWindow)
        return True
    def createSessionLog(self):
        datestamp = strftime("%m.%d.%Y - %H:%M:%S")
        datestamp = replace(datestamp, ":", "-")
        lf = open("log/"+datestamp + " session.log", "w")
        return lf
    """collects statistics about changed devices"""
    def collectData(self):
        self.equipdb_system = self.equipDB.getSystem() #get all devices, modelled as a system
        self.changed_systems = self.confDB.getChangedSystems(self.equipdb_system)
        self.new_devices_with_dhcp, self.new_devices_without_dhcp, self.changed_location_system, self.changed_dhcp_system, self.devices_up_to_date = self.changed_systems
#        for system in self.changed_systems:
#            print str(system)
    def UpdateMainFrameStatusPanel(self):
        wx.CallAfter(self.mainWindow.statusPanel.update)
    def UpdateMainFrameSparePanel(self):
        wx.CallAfter(self.mainWindow.sparePanel.update)
    def UpdateMainFrame(self):
        wx.CallAfter(self.UpdateMainFrameStatusPanel)
        wx.CallAfter(self.UpdateMainFrameSparePanel)
