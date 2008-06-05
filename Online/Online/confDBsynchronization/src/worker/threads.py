import wx
from threading import Thread
from database.emptydatabase import removeAllDevices

class EmptyThread(Thread):
    def __init__(self, progressFrame, controller):
        Thread.__init__(self)
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        removeAllDevices(self.controller, self.progressFrame)
        self.controller.collectData()
        self.progressFrame.OnFinish()
        self.controller.UpdateMainFrameStatusPanel()
        d2= wx.MessageDialog( None, "Deleting Devices finished!","Deleting devices finished!", wx.OK)
        d2.ShowModal() # Shows it
        d2.Destroy() # finally destroy it when finished.

class InsertThread(Thread):
    def __init__(self, progressFrame, controller):
        Thread.__init__(self)
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        self.controller.confDB.insertNewDevices(self.controller, self.progressFrame)
        self.controller.collectData()
        self.progressFrame.OnFinish()
        self.controller.UpdateMainFrameStatusPanel()
        d2= wx.MessageDialog( None, "Inserting new Devices finished!","Inserting new devices finished!", wx.OK)
        d2.ShowModal() # Shows it
        d2.Destroy() # finally destroy it when finished.

class SynchronizeThread(Thread):
    def __init__(self, progressFrame, controller):
        Thread.__init__(self)
        self.progressFrame = progressFrame
        self.controller = controller
    def run(self):
        self.controller.confDB.update(self.controller, self.progressFrame)
        self.controller.collectData()
        self.progressFrame.OnFinish()
        self.controller.UpdateMainFrameStatusPanel()
        d2= wx.MessageDialog( None, "Updating Devices finished!","Updating devices finished!", wx.OK)
        d2.ShowModal()
        d2.Destroy()
