from PyQt4.QtGui import QApplication
import sys
from mainwindowcontroller import MainWindowController

class Application(QApplication):
    def __init__(self):
        QApplication.__init__(self, sys.argv)
        self.mainWindowController = MainWindowController(self)
        self.exec_()

