import wx
from threading import Thread
from database.emptydatabase import removeAllDevices

class EmptyThread(Thread):
    def __init__(self, devices, progressFrame, controller):
        Thread.__init__(self)
        self.devices = devices
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        removeAllDevices(self.progressFrame, self.devices)
        self.devices.update()
        self.progressFrame.OnFinish()
        d2= wx.MessageDialog( None, "Deleting Devices finished!","Deleting devices finished!", wx.OK)
        d2.ShowModal() # Shows it
        d2.Destroy() # finally destroy it when finished.
        self.controller.UpdateMainFrameStatusPanel()

class InsertThread(Thread):
    def __init__(self, devices, progressFrame, controller):
        Thread.__init__(self)
        self.devices = devices
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        self.devices.insertNewDevices(self.progressFrame)
        self.devices.update()
        self.progressFrame.OnFinish()
        d2= wx.MessageDialog( None, "Inserting new Devices finished!","Inserting new devices finished!", wx.OK)
        d2.ShowModal() # Shows it
        d2.Destroy() # finally destroy it when finished.
        self.controller.UpdateMainFrameStatusPanel()

class SynchronizeThread(Thread):
    def __init__(self, devices, progressFrame, controller):
        Thread.__init__(self)
        self.devices = devices
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        self.devices.updateDHCP(self.progressFrame)
        self.devices.updateChangesOfLocation(self.progressFrame)
        self.devices.update()
        self.progressFrame.OnFinish()
        d2= wx.MessageDialog( None, "Updating Devices finished!","Updating devices finished!", wx.OK)
        d2.ShowModal()
        d2.Destroy()
        self.controller.UpdateMainFrameStatusPanel()
