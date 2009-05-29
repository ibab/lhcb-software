from PyQt4.QtCore import Qt, QObject, SIGNAL, SLOT, QVariant
from PyQt4.QtGui import QApplication, QMainWindow, QAction, QMessageBox, QStyle, QStyleFactory
from PyQt4 import uic

from CondDBUI import CondDB

from Models import CondDBStructureModel

import os

class MainWindow(QMainWindow):
    def __init__(self, parent = None, flags = Qt.Widget):
        super(MainWindow, self).__init__(parent, flags)
        self.model = None
        self.defaultDatabases = {} # action
        self._icons = None
        #self.setModel(CondDBStructureModel())
    
    def setModel(self, model):
        self.model = model
        self.model.icons = self.icons
        self.hierarchyTreeView.setModel(model)

    def setDefaultDatabases(self, connStrings):
        self.menuStandard.clear()
        self.defaultDatabases = {}
        names = connStrings.keys()
        names.sort()
        for name in names:
            conn = connStrings[name]
            action = QAction(self)
            action.setText(name)
            action.setData(QVariant(conn))
            action.setStatusTip(conn)
            QObject.connect(action, SIGNAL("triggered()"), self.openStandardDatabase)
            self.menuStandard.addAction(action)
            self.defaultDatabases[name] = action
        self.menuStandard.setEnabled(not self.menuStandard.isEmpty())
        
    def openStandardDatabase(self):
        sender = self.sender()
        #name = str(sender.text())
        #if name in self.defaultDatabases:
        #    self.model.connectDB(CondDB(self.defaultDatabases[name][1]))
        self.model.connectDB(CondDB(str(sender.data().toString())))

    def closeDatabase(self):
        self.model.connectDB(None)

    def aboutQt(self):
        QMessageBox.aboutQt(self)

    def aboutDialog(self):
        from PyQt4 import Qt
        app = QApplication.instance()
        message = '''
        %s %s

        Browser for the LHCb-COOL condition database.

        This is build on top of the Python API to LHCb-COOL: CondDBUI,
        and the Python API to COOL: PyCool.

        The Graphical Library is PyQt %s, based on Qt %s

        Marco Clemencic, Nicolas Gilardi
        '''%(app.objectName(), app.applicationVersion(), Qt.PYQT_VERSION_STR, Qt.qVersion())
        
        QMessageBox.about(self, app.objectName(), message)

    @property
    def icons(self, styleName = None):
        if styleName is None:
            styleName = os.environ.get("CONDDBBROWSER_STYLE", "Plastique")
        if self._icons is None:
            self._icons = {}
            style = QStyleFactory.create(styleName)
            if not style:
                print "Cannot find style '%s'," % styleName,
                styleName = QStyleFactory.keys()[0]
                print "trying '%s'." % styleName
                style = QStyleFactory.create(styleName)
            
            for iconName, iconId in [ ("folderset", QStyle.SP_DirIcon),
                                      ("folder", QStyle.SP_FileIcon) ]:
                self._icons[iconName] = style.standardIcon(iconId)
        return self._icons

def createMainWindow():
    mw = uic.loadUi(os.path.join(os.path.dirname(__file__),"MainWindow.ui"), MainWindow())
    mw.setModel(CondDBStructureModel())
    return mw
