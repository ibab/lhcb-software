from log.logReader import LogReader
import wx
import wx.richtext as rt
import os

class LogWindow(wx.Frame):
    def __init__(self, parentFrame = None, id = wx.ID_ANY, heading = "Showing Log File"):
        wx.Frame.__init__(self, parentFrame, wx.ID_ANY, heading, size=(780, 600))
        self.panel = wx.Panel(self, wx.ID_ANY, size=(780, 600))
        self.chooseFileButton = wx.Button(self.panel, -1, "Choose another log-file", pos=(10,10))
        self.Bind(wx.EVT_BUTTON, self.OnButton, self.chooseFileButton)
        self.logReader = LogReader()
        if heading == "Showing last Summary-Logfile":
            log = self.logReader.getLatestSummaryLogAsString()
        else:
            log = self.logReader.getLatestSessionLogAsString()
        self.logWidget = wx.TextCtrl(self.panel, -1, log, size=(760, 500), pos=(10, 40), style=wx.TE_MULTILINE|wx.TE_PROCESS_ENTER)
        self.logWidget.SetEditable(False)
        self.Show()
    """shows open file Dialog to choose the log-file manually"""
    def OnButton(self, e):
        wildcard = "Log-File (*.log)|*.log"
        dlg = wx.FileDialog(
            self, message="Choose a file",
            defaultDir=os.getcwd(), 
            defaultFile="",
            wildcard=wildcard,
            style=wx.OPEN | wx.MULTIPLE | wx.CHANGE_DIR
            )
        if dlg.ShowModal() == wx.ID_OK:
            paths = dlg.GetPaths()
            for path in paths:
                self.logWidget.SetValue(self.logReader.readFile(path))
            self.logWidget.Refresh()
            self.logWidget.Update()
        dlg.Destroy()

class SummaryLogWindow(LogWindow):
    def __init__(self, parent = None, id = wx.ID_ANY, heading = "Showing last Summary-Logfile"):
        LogWindow.__init__(self, parent, id, heading)

class SessionLogWindow(LogWindow):
    def __init__(self, parent = None, id = wx.ID_ANY, heading = "Showing last Session-Logfile"):
        LogWindow.__init__(self, parent, id, heading)

if __name__ == "__main__":
    app = wx.PySimpleApp()
    logWindow = SummaryLogWindow()
    app.MainLoop()