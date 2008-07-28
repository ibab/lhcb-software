from PyQt4 import QtCore, QtGui
from controller import Controller
from log.logReader import LogReader

class ShowLogWindowController(object):
    def __init__(self, parentController, summary):
        self.parentController = parentController
        self.logReader = LogReader()
        if summary:
            content = self.logReader.getLatestSummaryLogAsString()
        else:
            content = self.logReader.getLatestSessionLogAsString()
        self.textEdit = QtGui.QTextEdit(content)
        self.textEdit.setFixedSize(640, 480)
        self.textEdit.show()