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
        # Current selected (path, channel) in the database
        self._path = (None, None)
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
                       "tree": CondDBStructureModel(),
                       "nodes": CondDBNodesListModel(),
                       "tags": CondDBTagsListModel(),
                       "iovs": CondDBIoVModel(),
                       "fields": CondDBPayloadFieldModel(),
                       }
        setModelsIcons(self.icons)
        self.hierarchyTreeView.setModel(self.models["tree"])
        self.pathComboBox.setModel(self.models["nodes"])
        self.tagComboBox.setModel(self.models["tags"])
        self.iovView.setModel(self.models["iovs"])
        self.fieldsView.setModel(self.models["fields"])
        for m in self.models.values():
            QObject.connect(self, SIGNAL("openedDB"), m.connectDB)
            
        QObject.connect(self.hierarchyTreeView.selectionModel(), SIGNAL("currentChanged(QModelIndex,QModelIndex)"),
                        self.selectedItem)
        
        # special settings for the tags model
        tagsmodel = self.models["tags"]
        QObject.connect(self, SIGNAL("changedPath"), tagsmodel.setPath)
        QObject.connect(self.hideAutoCheckBox, SIGNAL("stateChanged(int)"), tagsmodel.setHideAutoTags)
        QObject.connect(tagsmodel, SIGNAL("setViewEnabled(bool)"), self.tagComboBox, SLOT("setEnabled(bool)"))
        QObject.connect(tagsmodel, SIGNAL("setViewEnabled(bool)"), self.hideAutoCheckBox, SLOT("setEnabled(bool)"))
        tagsmodel.setHideAutoTags(self.hideAutoCheckBox.checkState())
        # connection for the iovModel
        iovsmodel = self.models["iovs"]
        QObject.connect(self, SIGNAL("changedPathChannel"), iovsmodel.setPathChannel)
        QObject.connect(self.sinceFilterWidget, SIGNAL("validityKeyChange"), iovsmodel.setSince)
        QObject.connect(self.untilFilterWidget, SIGNAL("validityKeyChange"), iovsmodel.setUntil)
        QObject.connect(self.tagComboBox, SIGNAL("currentIndexChanged(QString)"), iovsmodel.setTag)
        QObject.connect(iovsmodel, SIGNAL("setViewEnabled(bool)"), self.iovView, SLOT("setEnabled(bool)"))
        self.iovView.setEnabled(False)
        QObject.connect(self.iovView.selectionModel(), SIGNAL("currentChanged(QModelIndex,QModelIndex)"),
                        iovsmodel.selectionChanged)
        QObject.connect(iovsmodel, SIGNAL("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"),
                        self.iovView.selectionModel(), SLOT("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"))
        
        # connection for the fields model
        fieldsmodel = self.models["fields"]
        QObject.connect(self, SIGNAL("changedPath"), fieldsmodel.setPath)
        #QObject.connect(fieldsmodel, SIGNAL("setViewEnabled(bool)"), self.fieldsView, SLOT("setEnabled(bool)"))
        #self.fieldsView.setEnabled(False)
        QObject.connect(fieldsmodel, SIGNAL("setViewEnabled(bool)"), self.payloadGroupBox, SLOT("setVisible(bool)"))
        self.payloadGroupBox.setVisible(False)
        
        QObject.connect(self.fieldsView.selectionModel(), SIGNAL("currentChanged(QModelIndex,QModelIndex)"),
                        fieldsmodel.selectionChanged)
        QObject.connect(fieldsmodel, SIGNAL("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"),
                        self.fieldsView.selectionModel(), SLOT("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"))
        
        # Filter panel
        # Default startup values for the IOV filter.
        self.sinceFilterWidget.setMaxEnabled(False)
        self.sinceFilterWidget.setDateTime(QDateTime.currentDateTime().addMonths(-1))
        #iovsmodel.setSince(self.sinceFilterWidget.toValidityKey())
        self.untilFilterWidget.setMaxChecked(True)
        #iovsmodel.setUntil(self.untilFilterWidget.toValidityKey())
        
        # When created, we have to ensure that everything is grayed out.
        self._setMainViewEnabled(False)
        self.menuEdit.setEnabled(False)
        self.menuAdvanced.setEnabled(False)
        
        # Triggers for the update of the central panel
        QObject.connect(self.iovView.selectionModel(), SIGNAL("currentChanged(QModelIndex,QModelIndex)"),
                        self.showData)
        QObject.connect(self.fieldsView.selectionModel(), SIGNAL("currentChanged(QModelIndex,QModelIndex)"),
                        self.showData)

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

    ## Commodity function to enable/disable the main widget (i.e. whatever needs
    #  to be grayed out when there is no database opened.
    def _setMainViewEnabled(self, value):
        value = bool(value)
        self.actionRead_Only.setEnabled(value)
        self.actionSlice.setEnabled(value)
        self.actionDump_to_files.setEnabled(value)
        self.actionClose.setEnabled(value)
        self.mainWidget.setEnabled(value)
    
    ## Open the database identified by a connection string or by a nickname.
    #  If name is an empty string (or None) the result is a disconnection from the
    #  current database.
    #
    #  @param connString connection string
    #  @param readOnly flag to select if the database has to be opened in read-only mode or in read/write mode
    def openDatabase(self, connString, readOnly = True):
        try:
            if connString:
                try:
                    self.db = CondDB(connString, readOnly = readOnly)
                except Exception, x:
                    msg = x.message
                    if msg.startswith("Database not found:"):
                        QMessageBox.critical(self, "Cannot open database", msg)
                        return
                    raise
                title = "%s - %s" % (connString, self.appName)
                self._setMainViewEnabled(True)
            else:
                self.db = None
                title = self.appName
                self._setMainViewEnabled(False)
            self.setWindowTitle(title)
            self._path = None
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
        if self._path != (item.path, item.channel):
            try:
                i = self.models["nodes"].nodes.index(item.path)
                self._path = (item.path, item.channel)
                self.emit(SIGNAL("changedPath"), item.path)
                if item.leaf and not item.children:
                    self.emit(SIGNAL("changedPathChannel"), item.path, item.channel)
                else:
                    self.emit(SIGNAL("changedPathChannel"), None, None)
            except ValueError:
                i = -1
            self.pathComboBox.setCurrentIndex(i)

    ## Slots to react to a selected entry in the path combo box
    def selectedPath(self, path):
        path = str(path)
        if self._path != (path, None):
            index = self.models["tree"].findPath(path)
            self.hierarchyTreeView.setCurrentIndex(index)
            self._path = (path, None)
            item = index.internalPointer()
            self.emit(SIGNAL("changedPath"), item.path)
            if item.leaf and not item.children:
                self.emit(SIGNAL("changedPathChannel"), path, None)
            else:
                self.emit(SIGNAL("changedPathChannel"), None, None)

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

    ## This function gets called if any selection changes and decides if there
    #  is something to show in the central widget.
    def showData(self):
        data = ""
        payload = self.models["iovs"].getPayload()
        if payload:
            field = self.models["fields"].getFieldName()
            # It may happen (it actually often does) that the function is
            # called for a change in the field before the IoVs are updated, so
            # it is better to check if the field exists in the payload.
            if field in payload:
                data = payload[field]
        self.dataView.setPlainText(data)

    ## Show a critical dialog with the latest exception traceback.
    #  @param source string representing the function that trapped the exception.
    def exceptionDialog(self, source = None):
        import traceback
        if source:
            msg = "Called from '%s':\n" % source
        else:
            msg = ""
        msg += traceback.format_exc()
        print msg
        QMessageBox.critical(self, "Exception in Python code", msg)
