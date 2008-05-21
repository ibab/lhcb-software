import wx
from string import split, replace

class SpareWindow(wx.Frame):
    def __init__(self, parentFrame = None, controller, id = wx.ID_ANY, heading = "Swap spare with device"):
        wx.Frame.__init__(self, parentFrame, wx.ID_ANY, heading, size=(600, 300))
        self.controller = controller
        self.panel = wx.Panel(self, wx.ID_ANY)
        self.chooseSpareStaticText = wx.StaticText(self.panel, wx.ID_ANY, "Choose spare:", pos=(20, 20))
        self.chooseDeviceStaticText = wx.StaticText(self.panel, wx.ID_ANY, "Choose device:", pos=(300, 20))
        self.CreateStatusBar()
        self.swapButton = wx.Button(self.panel, label="Swap selected spare with selected device", pos=(20, 200))
        self.Bind(wx.EVT_BUTTON, self.OnSwapButton, self.swapButton)
    def setDevices(self, devices):
        self.devices = devices
    def setSpares(self, spares):
        self.spares = spares
    def update(self):
        self.spareListBox = SimpleListBox(self.panel, self.spares, (20, 40))
        self.deviceListBox = SimpleListBox(self.panel, self.devices, (300, 40))
    def OnSwapButton(self, e):
        spare_index = self.spareListBox.GetSelection()
        device_index = self.deviceListBox.GetSelection()
        print spare_index, device_index
        spare = self.spares[spare_index]
        spare_hwname, spare_serialnb = spare.split(" (")
        spare_serialnb = spare_serialnb.replace(")", "")
        device = self.devices[device_index]
        device_hwname, device_serialnb = device.split(" (")
        device_serialnb = device_serialnb.replace(")", "")
        print spare_hwname, " :", spare_serialnb
        print device_hwname, " :", device_serialnb
        spare = self.controller.spareDB.getSpareConfDBByName(spare_hwname)
        self.controller.spareDB.replaceDevice(device_hwname, spare, self.controller.confDB)
        d= wx.MessageDialog( None, "Spare and device have been swapped","Success", wx.OK)
        d.ShowModal()
        d.Destroy()
        self.Show(False)
        self.Destroy()


class SimpleListBox(wx.ListBox):
    def __init__(self, parent, items, pos):
        wx.ListBox.__init__(self, parent, wx.ID_ANY, pos, (200, 100), items, wx.LB_SINGLE | wx.LB_SORT)
        if len(items) > 0:
            self.SetSelection(0)

if __name__ == "__main__":
    app = wx.PySimpleApp()
    spareWindow = SpareWindow()
    spareWindow.setDevices(["device1blabla (4ODLAUTL000154)", "device2"])
    spareWindow.setSpares(["spare1", "spare2", "spare21", "spare22", "spare23", "spare24", "spare25", "spare26", "spare27", "spare28", "spare29", "spare21", "spare2", "spare2", "spare2", "spare2", "spare2", "spare2", "spare2", "spare2", "spare2", "spare2"])
    spareWindow.update()
    spareWindow.Show(True)
    app.MainLoop()