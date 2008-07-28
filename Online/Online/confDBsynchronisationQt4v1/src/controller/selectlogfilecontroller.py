from controller import Controller
from PyQt4 import QtCore, QtGui
from log.logReader import LogReader

class SelectLogFileController(object):
    def __init__(self, parentController):
        self.parentController = parentController
        self.logReader = LogReader()
        fileName = QtGui.QFileDialog.getOpenFileName(None,
                                         "Choose logfile",
                                         "log",
                                         "Log Files (*.log);;All Files (*)")
        if not fileName.isEmpty():
            print fileName
        content = self.logReader.readFile(fileName)
        self.textEdit = QtGui.QTextEdit(content)
        self.textEdit.setFixedSize(640, 480)
        self.textEdit.show()