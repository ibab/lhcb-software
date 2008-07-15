from PyQt4 import QtGui

"""given a list with devices (UKL1, Tell1, Hugins or Spares) it will
   display information about the devices
"""
class ShowDevicesInfo(QtGui.QWidget):
    def __init__(self, devices, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.devices = devices