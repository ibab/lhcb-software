import os
import wx

from worker.threads import EmptyThread, InsertThread, SynchronizeThread
from spareWindow import SpareWindow
from logwindow import SessionLogWindow, SummaryLogWindow
from config import cDB_name, cDB_user, cDB_pass
#from model.devices import Devices
from model.device import Device
from worker.log import createLog

ID_ABOUT=101
ID_EXIT=110

ID_EMPTY=120
ID_INSERT=130
ID_SYNCHRONIZE=140
ID_TFCMUNIN01=150

ID_UPDATE_VIEW=160

ID_SUMMARY_LOG=170
ID_SHOW_SUMMARY_LOG=171
ID_SHOW_SESSION_LOG=172

ID_SPARE_INSERT=300
ID_SPARE_UPDATE=310
ID_SPARE_SWAP=320
ID_SPARE_DELETE=330

class MainWindow(wx.Frame):
    def __init__(self, controller, parentFrame = None, id = wx.ID_ANY, heading = "SyncTool v3"):
        wx.Frame.__init__(self, parentFrame, wx.ID_ANY, heading, size=(400, 350))
        self.heading = heading
        self.CreateStatusBar()
        self.statusPanel = StatusPanel(self, controller)
        self.sparePanel = SparePanel(self, controller)
        self.controller = controller
        filemenu= wx.Menu()
        filemenu.Append(ID_EMPTY, "&Delete all devices in ConfDB"," Deletes all devices and their connections from the Configuration Database")
        filemenu.Append(ID_INSERT, "&Insert New Devices"," Insert new devices from equipment database into configuration database")
        filemenu.Append(ID_SYNCHRONIZE, "&Update ConfDB"," Updates existing devices in configuration database")
        filemenu.Append(ID_TFCMUNIN01, "&Connect devices to TFCMUNIN01"," Connects all the hugin01 with TFCMUNIN01")
        filemenu.AppendSeparator()
        filemenu.Append(ID_ABOUT, "&About"," Information about this program")
        filemenu.AppendSeparator()
        filemenu.Append(ID_EXIT,"E&xit"," Terminate the program")
        editmenu = wx.Menu()
        editmenu.Append(ID_UPDATE_VIEW, "&Update View", "Refresh the View")
        logmenu = wx.Menu()
        logmenu.Append(ID_SUMMARY_LOG, "&Create Summary Log", "Creates a log file with a summary of changes")
        logmenu.Append(ID_SHOW_SESSION_LOG, "&Show last session-log", "Shows the latest session-log-file")
        logmenu.Append(ID_SHOW_SUMMARY_LOG, "S&how last summary-log", "Shows the latest summary-log-file")
        sparemenu = wx.Menu()
        sparemenu.Append(ID_SPARE_INSERT, "&Insert new spares", "Imports new spares from equipdb to confdb")
        sparemenu.Append(ID_SPARE_UPDATE, "&Update spares", "Updates existing spares in confdb with data from equipdb")
        sparemenu.Append(ID_SPARE_SWAP, "&Replace device", "Replace a device with a spare")
        sparemenu.Append(ID_SPARE_DELETE, "&Delete Spares", "Deletes all spares from configuration db")
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu,"&File")
        menuBar.Append(editmenu,"&Edit")
        menuBar.Append(logmenu, "&Log")
        menuBar.Append(sparemenu, "&Spare")
        self.SetMenuBar(menuBar)
        wx.EVT_MENU(self, ID_EMPTY, self.OnEmpty)
        wx.EVT_MENU(self, ID_INSERT, self.OnInsert)
        wx.EVT_MENU(self, ID_SYNCHRONIZE, self.OnSynchronize)
        wx.EVT_MENU(self, ID_TFCMUNIN01, self.OnTFCMUNIN01)
        #########################################
        wx.EVT_MENU(self, ID_ABOUT, self.OnAbout)
        #########################################
        wx.EVT_MENU(self, ID_EXIT, self.OnExit)
        #########################################
        #########################################
        wx.EVT_MENU(self, ID_SUMMARY_LOG, self.OnSummaryLog)
        wx.EVT_MENU(self, ID_SHOW_SUMMARY_LOG, self.OnShowSummaryLog)
        wx.EVT_MENU(self, ID_SHOW_SESSION_LOG, self.OnShowSessionLog)
        #########################################
        #########################################
        wx.EVT_MENU(self, ID_UPDATE_VIEW, self.OnUpdateView)
        #########################################
        #########################################
        wx.EVT_MENU(self, ID_SPARE_INSERT, self.OnInsertSpare)
        wx.EVT_MENU(self, ID_SPARE_UPDATE, self.OnUpdateSpare)
        wx.EVT_MENU(self, ID_SPARE_SWAP, self.OnSwapSpare)
        wx.EVT_MENU(self, ID_SPARE_DELETE, self.OnDeleteSpare)
        self.Show(True)
    def OnEmpty(self, e):
        progressFrame = ProgressFrame(self, "Deleting all Devices...", "Please be patient", len(self.controller.equipdb_system.getAllDevices()))
        emptyThread = EmptyThread( progressFrame, self.controller)
        emptyThread.start()
    def OnInsert(self, e):
        progressFrame = ProgressFrame(self, "Inserting new Devices...", "Please be patient", 2*(len(self.controller.new_devices_without_dhcp.getAllDevices())+len(self.controller.new_devices_with_dhcp.getAllDevices())))
        insertThread = InsertThread( progressFrame, self.controller)
        insertThread.start()
    def OnSynchronize(self, e):
        progressFrame = ProgressFrame(self, "Synchronizing Devices...", "Please be patient", 2*(len(self.controller.changed_location_system.getAllDevices())+len(self.controller.changed_dhcp_system.getAllDevices())))
        synchronizeThread = SynchronizeThread(progressFrame, self.controller)
        synchronizeThread.start()
    def OnTFCMUNIN01(self, e):
        self.controller.devices.connect_masterhugins()
        d= wx.MessageDialog( self, "Connecting the hugin01 to TFCMUNIN01 has finished","Finished ", wx.OK)
        d.ShowModal()
        d.Destroy()
    def OnAbout(self,e):
        d= wx.MessageDialog( self, "Synctool v3\nGUI Version","About "+self.heading, wx.OK)
        d.ShowModal() # Shows it
        d.Destroy() # finally destroy it when finished.
    def OnExit(self,e):
        self.Close(True)
    def OnSummaryLog(self, e):
        createLog(self.controller)
        d= wx.MessageDialog( self, "The summary log file was successfully created","Log ", wx.OK)
        d.ShowModal()
        d.Destroy()
    def OnUpdateView(self, e):
        self.controller.collectData()
        self.controller.UpdateMainFrame()
    def OnInsertSpare(self, e):
        self.controller.spareDB.InsertNewSpares()
        self.controller.UpdateMainFrameSparePanel()
    def OnUpdateSpare(self, e):
        self.controller.spareDB.UpdateSpares()
        self.controller.UpdateMainFrameSparePanel()
    def OnSwapSpare(self, e):
        spareWindow = SpareWindow(self, self.controller)
        spareWindow.setDevices(self.controller.confDB.getAllDevices())
        #TODO: all devices instead of spares?
        spareWindow.setSpares(self.controller.spareDB.getSparesInConfDBAsString())
        spareWindow.update()
        spareWindow.Show(True)
    def OnDeleteSpare(self, e):
        if self.controller.spareDB.DeleteSparesFromConfDB():
            d= wx.MessageDialog( self, "All spares have been deleted from the configuration database","Success", wx.OK)
        else:
            d= wx.MessageDialog( self, "There occurred errors during deleting all spares from the configuration database","Failure", wx.OK)
        d.ShowModal()
        d.Destroy()
        self.controller.UpdateMainFrameSparePanel()
    def OnShowSummaryLog(self, event):
        logWindow = SummaryLogWindow(self)
    def OnShowSessionLog(self, event):
        logWindow = SessionLogWindow(self)

class StatusPanel(wx.Panel):
    def __init__(self, parent, controller, id = wx.ID_ANY):
        wx.Panel.__init__(self, parent, id = wx.ID_ANY, size=(400, 139), pos=(0,0))
        self.parent = parent
        self.controller = controller
        #devices = self.controller.devices
        wx.StaticText(self, wx.ID_ANY, "New devices without DHCP Data:", pos=(10, 10))
        wx.StaticText(self, wx.ID_ANY, "New devices with DHCP Data:", pos=(10, 30))
        wx.StaticText(self, wx.ID_ANY, "New devices at all:", pos=(10, 50))
        wx.StaticText(self, wx.ID_ANY, "Changed devices:", pos=(10, 70))
        wx.StaticText(self, wx.ID_ANY, "Devices up-to-date:", pos=(10, 90))
        wx.StaticText(self, wx.ID_ANY, "Devices at all:", pos=(10, 110))
        ##############################################################################
        self.newDevicesWithoutDHCPText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.new_devices_without_dhcp.getAllDevices())), pos=(200, 10))
        self.newDevicesWithoutDHCPText.SetEditable(False)
        self.newDevicesWithDHCPText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.new_devices_with_dhcp.getAllDevices())), pos=(200, 30))
        self.newDevicesWithDHCPText.SetEditable(False)
        self.newDevicesAtAllText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.new_devices_without_dhcp.getAllDevices())+len(self.controller.new_devices_with_dhcp.getAllDevices())), pos=(200, 50))
        self.newDevicesAtAllText.SetEditable(False)
        self.DevicesWithNewDHCPText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.changed_location_system.getAllDevices())+len(self.controller.changed_dhcp_system.getAllDevices())), pos=(200, 70))
        self.DevicesWithNewDHCPText.SetEditable(False)
        self.newDevicesUpToDatePText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.devices_up_to_date.getAllDevices())), pos=(200, 90))
        self.newDevicesUpToDatePText.SetEditable(False)
        self.DevicesAtAllText = wx.TextCtrl(self, wx.ID_ANY, str(len(self.controller.equipdb_system.getAllDevices())), pos=(200, 110))
        self.DevicesAtAllText.SetEditable(False)
    def update(self):
        #devices = self.controller.devices
        #spareDB = self.controller.spareDB
        self.newDevicesWithoutDHCPText.ChangeValue(str(len(self.controller.new_devices_without_dhcp.getAllDevices())))
        self.newDevicesWithoutDHCPText.SetModified(True)
        self.newDevicesWithoutDHCPText.Refresh()
        self.newDevicesWithoutDHCPText.Update()
        self.newDevicesWithDHCPText.ChangeValue(str(len(self.controller.new_devices_with_dhcp.getAllDevices())))
        self.newDevicesWithDHCPText.SetModified(True)
        self.newDevicesWithDHCPText.Refresh()
        self.newDevicesWithDHCPText.Update()
        self.newDevicesAtAllText.ChangeValue(str(len(self.controller.new_devices_without_dhcp.getAllDevices())+len(self.controller.new_devices_with_dhcp.getAllDevices())))
        self.newDevicesAtAllText.SetModified(True)
        self.newDevicesAtAllText.Refresh()
        self.newDevicesAtAllText.Update()
        self.DevicesWithNewDHCPText.ChangeValue(str(len(self.controller.changed_location_system.getAllDevices())+len(self.controller.changed_dhcp_system.getAllDevices())))
        self.DevicesWithNewDHCPText.SetModified(True)
        self.DevicesWithNewDHCPText.Refresh()
        self.DevicesWithNewDHCPText.Update()
        self.newDevicesUpToDatePText.ChangeValue(str(len(self.controller.devices_up_to_date.getAllDevices())))
        self.newDevicesUpToDatePText.SetModified(True)
        self.newDevicesUpToDatePText.Refresh()
        self.newDevicesUpToDatePText.Update()
        self.DevicesAtAllText.ChangeValue(str(len(self.controller.equipdb_system.getAllDevices())))
        self.DevicesAtAllText.SetModified(True)
        self.DevicesAtAllText.Refresh()
        self.DevicesAtAllText.Update()
        self.Refresh()
        self.Update()
        self.parent.Refresh()
        self.parent.Update()

class SparePanel(wx.Panel):
    def __init__(self, parent, controller, id = wx.ID_ANY):
        wx.Panel.__init__(self, parent, id = wx.ID_ANY, size=(400, 150), pos=(0, 140))
        self.parent = parent
        self.controller = controller
        wx.StaticText(self, wx.ID_ANY, "New spares:", pos=(10, 30))
        wx.StaticText(self, wx.ID_ANY, "Changed spares:", pos=(10, 50))
        wx.StaticText(self, wx.ID_ANY, "Spares in ConfDB:", pos=(10, 70))
        new_spares_count = len(controller.spareDB.getNewSpares())
        print "new_spares_count: "+str(new_spares_count)
        self.NewSparesText = wx.TextCtrl(self, wx.ID_ANY, str(new_spares_count), pos=(200, 30))
        self.NewSparesText.SetEditable(False)
        changed_spares_count = len(controller.spareDB.getChangedSpares())
        print "changed_spares_count: "+str(changed_spares_count)
        self.ChangedSparesText = wx.TextCtrl(self, wx.ID_ANY, str(changed_spares_count), pos=(200, 50))
        self.ChangedSparesText.SetEditable(False)
        uptodate_spares_count = len(controller.spareDB.getSparesInConfDB())
        print "uptodate_spares_count: "+str(uptodate_spares_count)
        self.UpToDateSparesText = wx.TextCtrl(self, wx.ID_ANY, str(uptodate_spares_count), pos=(200, 70))
        self.UpToDateSparesText.SetEditable(False)
    def update(self):
        #devices = self.controller.devices
        spareDB = self.controller.spareDB
        new_spares_count = len(spareDB.getNewSpares())
        print "new_spares_count: "+str(new_spares_count)
        self.NewSparesText.ChangeValue(str(new_spares_count))
        self.NewSparesText.SetModified(True)
        self.NewSparesText.Refresh()
        self.NewSparesText.Update()
        changed_spares_count = len(spareDB.getChangedSpares())
        print "changed_spares_count: "+str(changed_spares_count)
        self.ChangedSparesText.ChangeValue(str(changed_spares_count))
        self.ChangedSparesText.SetModified(True)
        self.ChangedSparesText.Refresh()
        self.ChangedSparesText.Update()
        uptodate_spares_count = len(spareDB.getSparesInConfDB())
        print "uptodate_spares_count: "+str(uptodate_spares_count)
        self.UpToDateSparesText.ChangeValue(str(uptodate_spares_count))
        self.UpToDateSparesText.SetModified(True)
        self.UpToDateSparesText.Refresh()
        self.UpToDateSparesText.Update()
        self.Refresh()
        self.Update()
        self.parent.Refresh()
        self.parent.Update()

class ProgressFrame(wx.Frame):
    def __init__(self, parent, message, heading, max):
        wx.Frame.__init__(self, parent, wx.ID_ANY, heading, size=(400, 200))
        self.EnableCloseButton(False)
        self.panel = wx.Panel(self, wx.ID_ANY )
        self.messageText = wx.StaticText(self.panel, wx.ID_ANY, message, pos=(50, 50))
        self.gauge = wx.Gauge(self.panel, wx.ID_ANY, max, pos=(50, 100), size=(300, 30))
        self.Show(True)
    def update(self, value):
        self.gauge.SetValue(value)
    def OnFinish(self):
        self.EnableCloseButton(True)
        self.Destroy()