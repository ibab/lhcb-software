## @file MainWindow.py
#  Class for the main window.
#
#  Definition of the browser MainWindow class.

from PyQt4.QtCore import Qt, QObject, SIGNAL, SLOT, QVariant
from PyQt4.QtGui import QApplication, QMainWindow, QAction, QMessageBox, QStyle, QStyleFactory, QIcon
from PyQt4 import uic

from CondDBUI import CondDB

from Models import CondDBStructureModel

import os

## Class containing the logic of the application.
#  It should not be instantiated directly but only through the factory function createMainWindow().
#  @see createMainWindow().
class MainWindow(QMainWindow):
    ## Constructor.
    #  Initialises the base class, define some internal structures and set the icon of
    #  the window.
    def __init__(self, parent = None, flags = Qt.Widget):
        super(MainWindow, self).__init__(parent, flags)
        self.model = None
        self.defaultDatabases = {} # action
        self._icons = None
        self.setWindowIcon(QIcon(os.path.join(os.path.dirname(__file__),"icon.png")))

    ## Attach the item model for the database to the hierachy tree view.
    #  It also tells the model which icons to use.
    def setModel(self, model):
        self.model = model
        self.model.icons = self.icons
        self.hierarchyTreeView.setModel(model)

    ## Fills the menu of standard databases from the connString dictionary.
    #  @see getStandardConnectionStrings()
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
        
    def openStandardDatabase(self, name = None):
        if name is None:
            sender = self.sender()
        else:
            if name in self.defaultDatabases:
                sender = self.defaultDatabases[name]
            else:
                QMessageBox.critical(self, "Database nickname not known",
                                     "The conditions database '%s' is not in the list of known database." % name)
                return
        # Open the database using the connection string in the action
        self.model.connectDB(CondDB(str(sender.data().toString())))

    def closeDatabase(self):
        self.model.connectDB(None)

    def aboutQt(self):
        QMessageBox.aboutQt(self)

    def aboutDialog(self):
        from PyQt4 import Qt
        app = QApplication.instance()
        message = '''<p><b>%s</b><br/>%s</p>
        <p>Browser for the LHCb-COOL condition database.</p>
        <p>This is build on top of the Python API to LHCb-COOL: CondDBUI,
        and the Python API to COOL: PyCool.</p>
        <p>The Graphical Library is PyQt %s, based on Qt %s</p>
        <p><i>Marco Clemencic, Nicolas Gilardi</i></p>''' \
        % (app.objectName(), app.applicationVersion(), Qt.PYQT_VERSION_STR, Qt.qVersion())
        
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

## Factory function to create the main window.
#  The interface is dynamically loaded from the Qt-designer file MainWindow.ui.
def createMainWindow():
    mw = uic.loadUi(os.path.join(os.path.dirname(__file__),"MainWindow.ui"), MainWindow())
    mw.setModel(CondDBStructureModel())
    return mw
