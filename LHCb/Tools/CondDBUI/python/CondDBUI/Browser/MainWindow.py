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
        # Base class constructor.
        super(MainWindow, self).__init__(parent, flags)
        # Preconfigured databases
        self.defaultDatabases = {} # action
        # Icons for the model (property)
        self._icons = None
        # Set the window icon.
        self.setWindowIcon(QIcon(os.path.join(os.path.dirname(__file__),"icon.png")))
        # Prepare the GUI.
        uic.loadUi(os.path.join(os.path.dirname(__file__),"MainWindow.ui"), self)
        # Part of the initialization that require the GUI objects.
        self.model = CondDBStructureModel()
        self.model.icons = self.icons
        self.hierarchyTreeView.setModel(self.model)

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
        try:
            db = CondDB(str(sender.data().toString()))
            self.model.connectDB(db)
        except:
            self.exceptionDialog()

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

    ## Show a critical dialog with the latest exception traceback.
    #  @param source string representing the function that trapped the exception.
    def exceptionDialog(self, source = None):
        import traceback
        if source:
            msg = "Called from '%s':\n" % source
        else:
            msg = ""
        msg += traceback.format_exc()
        QMessageBox.critical(self, "Exception in Python code", msg)
