import wx
from view.mainwindow import MainWindow
from controller.controller import Controller
#app = wx.PySimpleApp()
controller = Controller()
controller.MainLoop()
#app.MainLoop()