## @file MainWindow.py
#  Class for the main window.
#
#  Definition of the browser MainWindow class.

from PyQt4.QtCore import (Qt, QObject,
                          SIGNAL, SLOT,
                          QVariant, QDateTime)
from PyQt4.QtGui import (QApplication, QMainWindow, QMessageBox,
                         QLabel,
                         QAction,
                         QIcon,
                         QStyle, QStyleFactory)
from PyQt4 import uic

from CondDBUI import CondDB

from Models import *

import os

## Class containing the logic of the application.
class MainWindow(QMainWindow):
    ## Constructor.
    #  Initialises the base class, define some internal structures and set the icon of
    #  the window.
    def __init__(self, parent = None, flags = Qt.Widget):
        # Base class constructor.
        super(MainWindow, self).__init__(parent, flags)
        # Application name
        app = QApplication.instance()
        self.appName = str(app.objectName())
        # Preconfigured databases
        self.defaultDatabases = {} # action
        # Icons for the model (property)
        self._icons = None
        # Current selected path in the database
        self._path = None
        # The database
        self.db = None
        # Current connection string. Needed because the copy in
        # self.db has the variables expanded.    
        self._connectionString = None
        # Prepare the GUI.
        uic.loadUi(os.path.join(os.path.dirname(__file__),"MainWindow.ui"), self)
        # --- Part of the initialization that require the GUI objects. ---
        # Window title
        self.setWindowTitle(self.appName)
        # Connect the models
        self.models = {
                       "tree":  CondDBStructureModel(),
                       "nodes": CondDBNodesListModel(),
                       "tags":  CondDBTagsListModel(),
                       }
        setModelsIcons(self.icons)
        self.hierarchyTreeView.setModel(self.models["tree"])
        self.pathComboBox.setModel(self.models["nodes"])
        self.tagComboBox.setModel(self.models["tags"])
        for m in self.models.values():
            QObject.connect(self, SIGNAL("openedDB"), m.connectDB)
        # special settings for the tags model
        tagsmodel = self.models["tags"]
        QObject.connect(self, SIGNAL("changedPath"), tagsmodel.setPath)
        QObject.connect(self.hideAutoCheckBox, SIGNAL("stateChanged(int)"), tagsmodel.setHideAutoTags)
        tagsmodel.setHideAutoTags(self.hideAutoCheckBox.checkState())

        # Filter panel
        # Default startup values for the IOV filter.
        self.sinceFilterWidget.setMaxEnabled(False)
        self.sinceFilterWidget.setDateTime(QDateTime.currentDateTime().addMonths(-1))
        self.untilFilterWidget.setMaxChecked(True)
        

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
    
    ## Slot called by the actions in the menu "Database->Standard".
    #  It can also be called passing the name of one of those databases.
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
        self.openDatabase(str(sender.data().toString()))        

    ## Open the database identified by a connection string or by a nickname.
    #  If name is an empty string (or None) the result is a disconnection from the
    #  current database.
    #
    #  @param connString connection string
    #  @param readOnly flag to select if the database has to be opened in read-only mode or in read/write mode
    def openDatabase(self, connString, readOnly = True):
        try:
            if connString:
                self.db = CondDB(connString, readOnly = readOnly)
                title = "%s - %s" % (connString, self.appName)
                self.actionRead_Only.setEnabled(True)
            else:
                self.db = None
                title = self.appName
                self.actionRead_Only.setEnabled(False)
            self.setWindowTitle(title)
            # Change the status of the editing menus
            editable = not readOnly
            self.menuEdit.setEnabled(editable)
            self.menuAdvanced.setEnabled(editable)
            self.actionRead_Only.setChecked(readOnly)
            # remember the used connection string
            self._connectionString = connString
            # update the DB instance of the models
            self.emit(SIGNAL("openedDB"), self.db)
        except:
            self.exceptionDialog()
    
    ## Re-open the current database, changing the readOnly flag.
    #  If the database is already in the correct mode, nothing is done.
    def reopenDatabase(self, readOnly):
        if readOnly != self.db.readOnly:
            self.openDatabase(self._connectionString, readOnly) 
    
    ## Disconnect from the database.
    #  @see: openDatabase()
    def closeDatabase(self):
        self.openDatabase(None)

    ## Display the standard Qt information dialog box
    def aboutQt(self):
        QMessageBox.aboutQt(self)

    ## Display the application information dialog box
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

    ## Slots to react to a selected item in the structure view
    def selectedItem(self, index):
        item = index.internalPointer()
        if item.path != self._path:
            try:
                i = self.models["nodes"].nodes.index(item.path)
                self._path = item.path
                self.emit(SIGNAL("changedPath"), self._path)
            except ValueError:
                i = -1
            self.pathComboBox.setCurrentIndex(i)

    ## Slots to react to a selected entry in the path combo box
    def selectedPath(self, path):
        path = str(path)
        if path != self._path:
            index = self.models["tree"].findPath(str(path))
            self.hierarchyTreeView.setCurrentIndex(index)
            self._path = path
            self.emit(SIGNAL("changedPath"), self._path)

    def testSlot(self):
        print "Test slot triggered"

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
