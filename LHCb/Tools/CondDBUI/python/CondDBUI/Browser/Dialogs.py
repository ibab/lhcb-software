## @file MainWindow.py
#  Class for the main window.
#
#  Definition of the browser MainWindow class.

from PyQt4.QtCore import (Qt, QObject, SIGNAL, SLOT,
                          QString,
                          QDateTime,
                          QRegExp)
from PyQt4.QtGui import (QDialog,
                         QFileDialog,
                         QProgressDialog,
                         QDialogButtonBox,
                         QMessageBox,
                         QValidator,
                         QRegExpValidator,
                         QHeaderView,
                         QItemSelectionModel,
                         QTextDocument)

from Models import (NodeFieldsModel, NodeFieldsDelegate,
                    CondDBTagsListModel, GlobalTagsListModel,
                    CondDBNodesListModel, CondDBPayloadFieldModel,
                    AddConditionsStackModel, CondDBSelectionsModel,
                    ChildTagDelegate, ChildTagsModel)

from Ui_NewDatabaseDialog import Ui_NewDatabaseDialog
from Ui_OpenDatabaseDialog import Ui_OpenDatabaseDialog
from Ui_NewNodeDialog import Ui_NewNodeDialog
from Ui_DumpToFilesDialog import Ui_DumpToFilesDialog
from Ui_AddConditionDialog import Ui_AddConditionDialog
from Ui_FindDialog import Ui_FindDialog
from Ui_EditConditionPayloadDialog import Ui_EditConditionPayloadDialog
from Ui_CreateSliceDialog import Ui_CreateSliceDialog
from Ui_SelectTagDialog import Ui_SelectTagDialog
from Ui_NewTagDialog import Ui_NewTagDialog

import os

__all__ = ["NewDatabaseDialog", "OpenDatabaseDialog", "NewNodeDialog",
           "DumpToFilesDialog", "ProgressDialog", "AddConditionDialog",
           "FindDialog", "CreateSliceDialog", "SelectTagDialog",
           "NewTagDialog"]

## Simple validator for COOL database name
class DBNameValidator(QRegExpValidator):
    ## Set the regular expression for valid COOL database name
    def __init__(self, parent):
        super(DBNameValidator, self).__init__(QRegExp("[A-Z][A-Z0-9_]{0,7}"), parent)
        self.isAcceptable = False
    ## Call the validate method QRegExpValidator and remember the result
    def validate(self, input, pos):
        res = QRegExpValidator.validate(self, input, pos)
        self.isAcceptable = res[0] == QValidator.Acceptable
        return res

## Dialog to collect information to create a new database.
class NewDatabaseDialog(QDialog, Ui_NewDatabaseDialog):
    ## Constructor.
    #  Initializes the base class and defines some internal structures.
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(NewDatabaseDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        self.dbNameValidator = DBNameValidator(self)
        self.partitionComboBox.setValidator(self.dbNameValidator)
        # This is needed to ensure that the internal state of the validator is up to date 
        self.dbNameValidator.validate(self.partitionComboBox.currentText(), 0)
        # Ensure that the Ok button is in the correct state
        self.checkValid()
    ## Slot used to execute a dialog to select the filename 
    def openFileDialog(self):
        name = QFileDialog.getSaveFileName(self, "Database file", os.getcwd(), "*.db")
        if name:
            self.filenameEdit.setText(name)
    ## Check if the inputs are suitable for a connection string
    def validInputs(self):
        return not self.filenameEdit.text().isEmpty() and self.dbNameValidator.isAcceptable
    ## Return the current connection string implied by the content of the dialog
    def connectionString(self):
        if self.validInputs():
            cs = "sqlite_file:%s/%s" % (self.filenameEdit.text(), self.partitionComboBox.currentText())
        else:
            cs = "" 
        return cs
    ## Slot to set the state of the Ok button depending on the data inserted
    def checkValid(self):
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(self.validInputs())


## Dialog to collect the information to open a database.
class OpenDatabaseDialog(QDialog, Ui_OpenDatabaseDialog):
    ## Constructor.
    #  Initializes the base class and defines some internal structures.
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(OpenDatabaseDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        self.dbNameValidator = DBNameValidator(self)
        self.partitionComboBox.setValidator(self.dbNameValidator)
        # This is needed to ensure that the internal state of the validator is up to date 
        self.dbNameValidator.validate(self.partitionComboBox.currentText(), 0)
        # Ensure that the Ok button is in the correct state
        self.checkValid()
    ## Slot used to execute a dialog to select the filename 
    def openFileDialog(self):
        name = QFileDialog.getOpenFileName(self, "Database file", os.getcwd(), "*.db")
        if name:
            self.filenameEdit.setText(name)
    ## Check if the inputs are suitable for a connection string
    def validInputs(self):
        valid = False
        currIndex = self.tabWidget.currentIndex()
        if currIndex == 0:
            valid = not self.filenameEdit.text().isEmpty() and self.dbNameValidator.isAcceptable
        elif currIndex == 1:
            valid = not self.connStringEdit.text().isEmpty()
        return valid
    ## Return the current connection string implied by the content of the dialog
    def connectionString(self):
        if self.validInputs():
            if self.tabWidget.currentIndex() == 0:
                cs = "sqlite_file:%s/%s" % (self.filenameEdit.text(), self.partitionComboBox.currentText())
            else:
                cs = self.connStringEdit.text()
        else:
            cs = ""
        return cs
    ## Return the status of the check box for read-only or read/write access
    def readOnly(self):
        return self.readOnlyCheckBox.isChecked()
    ## Slot to set the state of the Ok button depending on the data inserted
    def checkValid(self):
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(self.validInputs())

## Simple validator to check if the node name specified is allowed
class NodeNameValidator(QValidator):
    ## Initializes internal data structures
    def __init__(self, parent):
        super(NodeNameValidator, self).__init__(parent)
        self._createParents = True
        self.lastState = QValidator.Intermediate
        self.folders = set()
        self.foldersets = set()
    def setCreateParents(self, create):
        self._createParents = create
    def createParents(self):
        return self._createParents
    def validate(self, input, pos):
        path = unicode(input).rstrip("/")
        state = QValidator.Acceptable
        # Check that it is not empty
        if not path or path[0] != '/':
            state = QValidator.Invalid
        # look if the name is already used
        elif path in self.folders or path in self.foldersets:
            state = QValidator.Intermediate
        else:
            # check the parent
            parent = path.rsplit('/',1)[0]
            if parent: # empty parent means it is based at '/', so it's ok
                if parent in self.folders: # the parent cannot be a folder
                    state = QValidator.Invalid
                elif not self._createParents and parent not in self.foldersets:
                    # the parent has to be a folderset if we do not want to create the parents
                    state = QValidator.Invalid
        self.lastState = state
        return state, pos

## Dialog to create a new node.
class NewNodeDialog(QDialog, Ui_NewNodeDialog):
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(NewNodeDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        self.validator = NodeNameValidator(self)
        self.validator.setCreateParents(self.createParentsCheckBox.isChecked())
        QObject.connect(self.createParentsCheckBox, SIGNAL("toggled(bool)"), self.validator.setCreateParents)
        self.nodeNameComboBox.setValidator(self.validator)
        self.fieldsModel = NodeFieldsModel(parent = self.fieldsView)
        self.fieldsView.setModel(self.fieldsModel)
        self.fieldsDelegate = NodeFieldsDelegate(self)
        self.fieldsView.setItemDelegateForColumn(1, self.fieldsDelegate)
        # Ensure the initial state of the dialog
        self.multiVersBtn.setChecked(True)
        self.editFieldsBtn.setChecked(False)
        self.fieldsGB.hide()
        self.checkValid()
    ## Set the list of folders and foldersets for the combo box
    def setNodes(self, folders, foldersets):
        nodes = list(folders) + list(foldersets)
        nodes.sort()
        self.nodeNameComboBox.clear()
        self.nodeNameComboBox.addItems(nodes)
        self.validator.folders = set(folders)
        self.validator.foldersets = set(foldersets)
    ## Set the value of the text field
    def setText(self, text):
        self.nodeNameComboBox.setEditText(text)
    ## Get the value of the text field
    def text(self):
        return self.nodeNameComboBox.currentText()
    ## Get the value of the description field field
    def description(self):
        return self.descriptionEdit.text()
    ## Get the type of node that has been selected
    def getType(self):
        if self.multiVersBtn.isChecked():
            return "MultiVersion"
        elif self.singleVersBtn.isChecked():
            return "SingleVersion"
        else:
            return "FolderSet"
    ## Get the list of defined fields for the folder
    def fields(self):
        return self.fieldsModel.fields
    ## Check if the inputs are suitable for a connection string
    def validInputs(self):
        return self.validator.lastState == QValidator.Acceptable
    ## Slot to set the state of the Ok button depending on the data inserted
    def checkValid(self):
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(self.validInputs())
    ## Slot to handle the toggling of the folderset radio button
    def foldersetButtonToggled(self, state):
        self.editFieldsBtn.setDisabled(state)
        if state:
            self.fieldsGB.hide()
        else:
            self.fieldsGB.setVisible(self.editFieldsBtn.isChecked())
    ## Slot used to add a new field to the field view
    def addField(self):
        self.fieldsModel.insertRow(self.fieldsModel.rowCount())
    ## Slot used to remove a field from the field view
    def removeField(self):
        if self.fieldsView.selectionModel().hasSelection():
            index = self.fieldsView.selectionModel().currentIndex()
            self.fieldsModel.removeRow(index.row())

## Dialog to dump a snapshot to files.
class DumpToFilesDialog(QDialog, Ui_DumpToFilesDialog):
    ## Constructor.
    #  Initializes the base class and defines some internal structures.
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(DumpToFilesDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        # @todo: the distinction between local tags and global tag is not yet implemented
        self.localTags.hide()
        self.tagsModel = GlobalTagsListModel(self)
        self.tag.setModel(self.tagsModel)
        # Initialize fields
        self.destDir.setText(os.getcwd())
        self.pointInTime.setDateTime(QDateTime.currentDateTime())
    ## Open a directory selection dialog
    def selectDirectory(self):
        name = QFileDialog.getExistingDirectory(self, "Destination directory", os.getcwd())
        if name:
            self.destDir.setText(name)
    ## Show or hide local tags in the tag combo box
    def showLocalTags(self, show):
        pass

## Small extension to the standard QProgressDialog to show only string up to a
#  limited size.
class ProgressDialog(QProgressDialog):
    ## Override QProgressDialog::setLabelText to limit the number of chars
    #  displayed in the message.
    def setLabelText(self, text):
        # TODO: the limit should depend on the rendered size and not on the
        # number of chars. 
        if len(text) > 40:
            QProgressDialog.setLabelText(self, "..." + text[-37:])
        else:
            QProgressDialog.setLabelText(self, text)

_xml_template_empty = """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>
</DDDB>
"""

## Editor of conditions payloads
class EditConditionPayloadDialog(QDialog, Ui_EditConditionPayloadDialog):
    ## Constructor.
    def __init__(self, data, parent = None, flags = Qt.Dialog, externalEditor = None):
        # Base class constructor.
        super(EditConditionPayloadDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        if not data:
            raise RuntimeError("Cannot start EditConditionPayloadDialog with empty data")
        self.data = dict(data)
        self._field = None
        self._externalEditor = externalEditor
        if not self._externalEditor:
            self.externalEditorButton.hide()
        else:
            self.externalEditorButton.setText("Open in %s" % self._externalEditor)
        # useful to protect the updateData method when selectField is selected
        self._selectingField = False
        keys = self.data.keys()
        keys.sort()
        self.fields.addItems(keys)
        self.fields.setEnabled(len(keys) > 1)
        self.selectField(keys[0])
    ## Fill the editor text from a file
    def importFromFile(self):
        filename = QFileDialog.getOpenFileName(self)
        if filename:
            data = open(str(filename)).read()
            self.editor.setPlainText(data)
    ## Save the current content of the text box to a file
    def exportToFile(self):
        filename = QFileDialog.getSaveFileName(self)
        if filename:
            xmlFile = open(str(filename), 'w')
            xmlFile.write(str(self.editor.toPlainText()))
            xmlFile.close()
    ## Save the content of the editor to a temporary file and open it with an
    #  external editor. 
    def openInExternalEditor(self):
        import tempfile
        fd, name = tempfile.mkstemp(suffix = ".xml")
        os.fdopen(fd, "w").write(str(self.editor.toPlainText()))
        try:
            from subprocess import Popen
            Popen([self._externalEditor, name]).wait()
            data = open(name).read()
            self.editor.setPlainText(data)
        finally:
            os.remove(name)
    ## insert text at the current cursor position in the editor
    def _insertText(self, text, *args):
        if args:
            text = text % args
        self.editor.textCursor().insertText(text)
    ## Insert a default condition tag at the current cursor position
    def insertCondition(self):
        class_id = 5
        name = "##_CONDITION_NAME_HERE_##"
        text = '<condition classID="%d" name="%s">\n\n</condition>'
        self._insertText(text, class_id, name)
    ## Insert a default alignment condition tag at the current cursor position
    def insertAlignmentCondition(self):
        name = "##_CONDITION_NAME_HERE_##"
        text = """<condition classID="6" name="%s">
<paramVector name="dPosXYZ" type="double">0 0 0</paramVector>
<paramVector name="dRotXYZ" type="double">0 0 0</paramVector>
<paramVector name="pivotXYZ" type="double">0 0 0</paramVector>
</condition>"""
        self._insertText(text, name)
    ## Insert a default param tag at the current cursor position
    def insertParam(self):
        name = "##_PARAM_NAME_HERE_##"
        type_id = '##_INT_DOUBLE_STRING_##'
        value = ''
        text = '<param name="%s" type="%s">%s</param>'
        self._insertText(text, name, type_id, value)
    ## Insert a default paramvector tag at the current cursor position
    def insertParamVector(self):
        name = "##_PARAM_NAME_HERE_##"
        type_id = '##_INT_DOUBLE_STRING_##'
        value = ''
        text = '<paramVector name="%s" type="%s">%s</paramVector>'
        self._insertText(text, name, type_id, value)
    ## Change the content of the edit box when another field is selected
    def selectField(self, field):
        self._field = field = str(field)
        if field in self.data:
            if self.fields.currentText() != field:
                idx = self.fields.findText(field)
                self.fields.setCurrentIndex(idx)
            self.editor.setDocumentTitle(field)
            self._selectingField = True # protect the updateData function
            if self.data[field]: 
                self.editor.setPlainText(self.data[field])
            else:
                # if the data is empty, use the template
                self.editor.setPlainText(_xml_template_empty)
            self._selectingField = False
    ## Update the internal buffer with the content of the editor box
    def updateData(self):
        if not self._selectingField:
            self.data[self._field] = str(self.editor.toPlainText())

## Dialog to prepare the insertion of new conditions
class AddConditionDialog(QDialog, Ui_AddConditionDialog):
    #__pyqtSignals__ = ("folderChanged(QString)")
    ## Constructor.
    #  Initializes the base class and defines some internal structures.
    def __init__(self, parent = None, flags = Qt.Dialog, externalEditor = None):
        # Base class constructor.
        super(AddConditionDialog, self).__init__(parent, flags)
        self.db = parent.db
        self.folderModel = CondDBNodesListModel(self.db, parent = self,
                                                nodeType = CondDBNodesListModel.FOLDER)
        self.fieldsModel = CondDBPayloadFieldModel(self.db, parent = self)
        self.conditionsStack = AddConditionsStackModel(parent = self)
        self._externalEditor = externalEditor # to be passed to the edit dialog
        # Prepare the GUI.
        self.setupUi(self)
        self.folder.setModel(self.folderModel)
        self.fields.setModel(self.fieldsModel)
        self.conditionsStackView.setModel(self.conditionsStack)
        self.conditionsStackView.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.folder.setCurrentIndex(-1)
        self.until.setMaxChecked(True)
        self.since.setToNow()
        self.since.setMaxEnabled(False)
        self.channel.setText("0")
        self._updateBuffer()
        # Bind signals and slots
        QObject.connect(self.folder, SIGNAL("currentIndexChanged(QString)"),
                        self.fieldsModel.setPath)
        QObject.connect(self.fieldsModel, SIGNAL("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"),
                        self.fields.selectionModel(), SLOT("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"))
        QObject.connect(self.utc, SIGNAL("stateChanged(int)"),
                        self.conditionsStack.setShowUTC)
        QObject.connect(self.conditionsStackView.selectionModel(),
                        SIGNAL("selectionChanged(QItemSelection,QItemSelection)"),
                        self.changedSelection)
        QObject.connect(self.conditionsStack, SIGNAL("conflictsChanged(bool)"),
                        self.buttonBox.button(QDialogButtonBox.Ok), SLOT("setDisabled(bool)"))
        # Use a consistent DisplayFormat
        self.conditionsStack.setShowUTC(self.utc.checkState())
        self.conditionsStack.setDisplayFormat(self.since.displayFormat())
    
    def _updateBuffer(self):
        # prepare the buffer
        self.buffer = {}
        for f in self.fieldsModel.getFieldNames():
            self.buffer[f] = ""
        
    ## Set the value of the channel field.
    def setChannel(self, ch):
        if ch or ch == 0:
            ch = str(ch)
        else:
            ch = ""
        self.channel.setText(ch)
    
    ## Get the value of channel field.
    def getChannel(self):
        return str(self.channel.text())
    
    ## Set the value of the folder field.
    def setFolder(self, folder):
        try:
            self.folder.setCurrentIndex(self.folderModel.nodes.index(str(folder)))
            self._updateBuffer()
        except ValueError:
            pass # This may happen when the user selected a folderset
    
    ## Set the value of the folder field.
    def getFolder(self):
        return str(self.folder.currentText())
    
    ## Set folder and channel
    def setLocation(self, folder, channel):
        self.setFolder(folder)
        self.setChannel(channel)
        
    ## ShowUTC property
    def setShowUTC(self, value):
        if value != self.getShowUTC():
            self.utc.setChecked(value)
            self.conditionsStack.setShowUTC(value)
    ## ShowUTC property
    def getShowUTC(self):
        return self.utc.checkState()
    
    ## Show message box with instructions for the dialog.
    def showHelp(self):
        mb = QMessageBox(self)
        mb.setText("Add a condition")
        mb.setInformativeText("""<html><body><ol>
<li>select a location (folder and channel)</li>
<li>set the Interval of Validity (since and until)</li>
<li>select a payload key and click on the button "Edit" to modify it; repeat for all the keys</li>
<li>click on the add button (+) to add the edited condition object to the buffer</li>
<li>repeat steps 2-4 for all the intervals of validity needed</li>
<li>click on the "Ok" button to commit the changes to the database</li>
</ol></body></html>""")
        mb.setStandardButtons(QMessageBox.Ok)
        mb.exec_()

    ## Add the edited condition to the stack
    def addCondition(self):
        self.conditionsStack.addCondition(since   = self.since.toValidityKey(),
                                          until   = self.until.toValidityKey(),
                                          channel = self.getChannel(),
                                          data    = self.buffer)
    ## Remove the selected condition from the stack
    def removeCondition(self):
        selection = self.conditionsStackView.selectedIndexes()
        self.conditionsStack.removeConditions(selection)
    ## Move the selected condition up in the stack
    def moveUp(self):
        selection = set([i.row() for i in self.conditionsStackView.selectedIndexes()])
        if len(selection) == 1:
            row = selection.pop()
            self.conditionsStack.moveUp(row)
            row -= 1
            if row >= 0:
                index = self.conditionsStack.index(row, 0)
                selmodel = self.conditionsStackView.selectionModel()
                selmodel.select(index,
                                QItemSelectionModel.ClearAndSelect | QItemSelectionModel.Rows)
    ## Move the selected condition down in the stack
    def moveDown(self):
        selection = set([i.row() for i in self.conditionsStackView.selectedIndexes()])
        if len(selection) == 1:
            row = selection.pop()
            self.conditionsStack.moveDown(row)
            row += 1
            if row < self.conditionsStack.rowCount():
                index = self.conditionsStack.index(row, 0)
                selmodel = self.conditionsStackView.selectionModel()
                selmodel.select(index,
                                QItemSelectionModel.ClearAndSelect | QItemSelectionModel.Rows)
    def changedSelection(self, newSelection, oldSelection):
        rows = self.conditionsStackView.selectedRows()
        count = len(rows)
        self.upButton.setEnabled(count == 1)
        self.downButton.setEnabled(count == 1)
        self.removeButton.setEnabled(count != 0)
        if count == 1:
            row = rows[0].row()
            cond = self.conditionsStack[row]
            self.since.setValidityKey(cond.since)
            self.until.setValidityKey(cond.until)
            self.setChannel(cond.channel)
            self.buffer = dict(cond.data) # I make a copy for safety
    ## Display the condition editor
    def editCondition(self):
        d = EditConditionPayloadDialog(self.buffer, self,
                                       externalEditor = self._externalEditor)
        d.selectField(self.fieldsModel.getFieldName())
        if d.exec_():
            self.buffer.update(d.data)

## Simple "find" dialog.
class FindDialog(QDialog, Ui_FindDialog):
    __pyqtSignals__ = ("find(QString,QTextDocument::FindFlags,bool)")
    ## Constructor.
    #  Initializes the base class and defines some internal structures.
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(FindDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        # data members
        self._text = QString()
        self._findFlags = QTextDocument.FindFlags()
        self._wrappedSearch = True
        self._updateCheckBoxes()
    
    ## Text property
    def setText(self, value):
        self._text = QString(value)
    ## Text property
    def text(self):
        return self._text
    
    ## Helper function to set/unset a bit of findFlags
    def _setFlagBit(self, bit, value):
        if value:
            self._findFlags |= bit
        else:
            self._findFlags ^= self._findFlags & bit
        self._updateCheckBoxes()
    ## Helper function to check the status of a bit of findFlags
    def _getFlagBit(self, bit):
        return bool(self._findFlags & bit)
    
    ## Helper function to update the checkboxes
    def _updateCheckBoxes(self):
        self.caseSensitive.setChecked(self.getCaseSensitive())
        self.wholeWord.setChecked(self.getWholeWord())
        self.wrappedSearch.setChecked(self.getWrappedSearch())
        self.searchBackward.setChecked(self.getBackwardSearch())
    
    ## CaseSensitive property
    def setCaseSensitive(self, value):
        self._setFlagBit(QTextDocument.FindCaseSensitively, value)
    ## CaseSensitive property
    def getCaseSensitive(self):
        return self._getFlagBit(QTextDocument.FindCaseSensitively)
    
    ## WholeWord property
    def setWholeWord(self, value):
        self._setFlagBit(QTextDocument.FindWholeWords, value)
    ## WholeWord property
    def getWholeWord(self):
        return self._getFlagBit(QTextDocument.FindWholeWords)
    
    ## BackwardSearch property
    def setBackwardSearch(self, value):
        self._setFlagBit(QTextDocument.FindBackward, value)
    ## BackwardSearch property
    def getBackwardSearch(self):
        return self._getFlagBit(QTextDocument.FindBackward)
    
    ## FindFlags property
    def setFindFlags(self, value):
        if type(value) is QTextDocument.FindFlags:
            self._findFlags = value
        else:
            self._findFlags = QTextDocument.FindFlags()
            for bit in (QTextDocument.FindBackward, QTextDocument.FindCaseSensitively, QTextDocument.FindWholeWords):
                if value & int(bit):
                    self._findFlags |= bit 
        self._updateCheckBoxes()
    ## FindFlags property
    def getFindFlags(self):
        return self._findFlags

    ## WrappedSearch property
    def setWrappedSearch(self, value):
        self._wrappedSearch = value
        self.wrappedSearch.setChecked(value)
    ## WrappedSearch property
    def getWrappedSearch(self):
        return self._wrappedSearch

    ## Slot to emit the signal to start the search    
    def doFind(self):
        self.emit(SIGNAL("find(QString,QTextDocument::FindFlags,bool)"),
                  self._text, self._findFlags, self._wrappedSearch)

## Dialog to create a snapshot of the database
class CreateSliceDialog(QDialog, Ui_CreateSliceDialog):
    ## Constructor.
    def __init__(self, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(CreateSliceDialog, self).__init__(parent, flags)
        # Prepare the GUI.
        self.setupUi(self)
        # Validation of the database name
        self.dbNameValidator = DBNameValidator(self)
        self.partition.setValidator(self.dbNameValidator)
        #   This is needed to ensure that the internal state of the validator is up to date 
        self.dbNameValidator.validate(self.partition.currentText(), 0)
        # Models
        #   tags
        self.tagsModel = CondDBTagsListModel(self)
        self.tags.setModel(self.tagsModel)
        QObject.connect(self.tags.selectionModel(), SIGNAL("selectionChanged(QItemSelection,QItemSelection)"),
                        self.tagsModelSelectionChanged)
        #   paths
        self.pathModel = CondDBNodesListModel(parent.db, self, needRoot = True)
        self.path.setModel(self.pathModel)
        #   selections
        self.selectionsModel = CondDBSelectionsModel(self)
        self.selectionsModel.setDisplayFormat(self.since.displayFormat())
        self.selections.setModel(self.selectionsModel)
        self.selections.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        self.utc.setChecked(self.selectionsModel.showUTC())
        QObject.connect(self.utc, SIGNAL("stateChanged(int)"),
                        self.selectionsModel.setShowUTC)
        QObject.connect(self.selections.selectionModel(), SIGNAL("selectionChanged(QItemSelection,QItemSelection)"),
                        self.selModelSelectionChanged)
        # UI tuning
        self.since.setMaxEnabled(False)
        # Force a default initial selection
        self.path.setCurrentIndex(0)
        self.tagsModel.setPath(self.pathModel.nodes[0])
        self.tags.selectionModel().select(self.tagsModel.index(0),
                                          QItemSelectionModel.ClearAndSelect)
        self.since.setToMinimum()
        self.until.setMaxChecked(True)
        # Ensure UI consistency
        self.checkValidData()
        self.selModelSelectionChanged()
        self.tagsModelSelectionChanged()
    ## Slot to set the state of the Ok button depending on the data inserted
    def checkValidData(self):
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(self.validInputs())
    ## Check if the inputs are suitable for a connection string
    def validInputs(self):
        return not self.filename.text().isEmpty() \
                and self.dbNameValidator.isAcceptable \
                and self.selectionsModel.rowCount()
    ## Return the current connection string implied by the content of the dialog
    def connectionString(self):
        if self.validInputs():
            cs = "sqlite_file:%s/%s" % (self.filename.text(), self.partition.currentText())
        else:
            cs = "" 
        return cs
    ## Slot reacting to a change in the selection in the selections list to
    #  enable/disable the remove button
    def selModelSelectionChanged(self):
        rows = self.selections.selectionModel().selectedRows()
        self.removeButton.setEnabled(len(rows) == 1)   
    ## Slot reacting to a change in the selection in the tags list to
    #  enable/disable the add button
    def tagsModelSelectionChanged(self):
        count = len(self.tags.selectedIndexes())
        self.addButton.setEnabled(count != 0)   
    ## Slot used to execute a dialog to select the filename 
    def openFileDialog(self):
        name = QFileDialog.getSaveFileName(self, "Database file", os.getcwd(), "*.db")
        if name:
            self.filename.setText(name)
    ## Add the data for a PyCoolCopy.Selection to the list of selections
    def addSelection(self):
        path = str(self.path.currentText())
        since = self.since.toValidityKey()
        until = self.until.toValidityKey()
        tags = []
        for i in self.tags.selectedIndexes():
            tags.append(str(self.tagsModel.tags()[i.row()][0]))
        self.selectionsModel.addSelection(path, since, until, tags)
        self.checkValidData()
    ## Remove an entry from the list of selections
    def removeSelection(self):
        rows = self.selections.selectionModel().selectedRows()
        if len(rows) == 1:
            self.selectionsModel.removeSelection(rows[0].row())
        self.checkValidData()
    ## Slot triggered by a change in the node path combobox. 
    def currentPathSelected(self, path):
        self.tags.selectionModel().clear()
        self.tagsModel.setPath(path)

## Simple dialog to select a tag from a list
class SelectTagDialog(QDialog, Ui_SelectTagDialog):
    ## Constructor.
    def __init__(self, path, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(SelectTagDialog, self).__init__(parent, flags)
        self._path = path
        # Prepare the GUI.
        self.setupUi(self)
        self.tagModel = CondDBTagsListModel(path = self._path)
        self.tag.setModel(self.tagModel)
    ## Returns the selected tag.
    def getSelected(self):
        return str(self.tag.currentText())
    ## Executes the dialog and return the selected tag or None if canceled.
    def run(self):
        if self.exec_():
            return self.getSelected()
        return None

class NewTagDialog(QDialog, Ui_NewTagDialog):
    ## Constructor.
    def __init__(self, db, path, parent = None, flags = Qt.Dialog):
        # Base class constructor.
        super(NewTagDialog, self).__init__(parent, flags)
        self._path = path
        self._db = db
        self._isFolderSet = self._db.db.existsFolderSet(self._path)
        # Prepare the GUI.
        self.setupUi(self)
        self.node.setText(self._path)
        # If we are dealing with a folder, we do not need the selection of
        # child tags.
        self.setChildTagsPartVisible(self._isFolderSet)
        if self._isFolderSet:
            self.childTagsDelegate = ChildTagDelegate(self, self._path)
            self.childTagsModel = ChildTagsModel(self._db , self._path, self)
            self.childTags.setModel(self.childTagsModel)
            self.childTags.setItemDelegateForColumn(1, self.childTagsDelegate)
            self.childTags.horizontalHeader().setResizeMode(QHeaderView.ResizeToContents)
        # Ensure consistency of the UI
        self.checkValidData()
    def checkValidData(self):
        ok = not self.tag.text().isEmpty()
        self.buttonBox.button(QDialogButtonBox.Ok).setEnabled(ok)
    def setChildTagsPartVisible(self, visible):
        for w in [self.childTags, self.childTagsLabel, self.fillChildTagsBtn]:
            w.setVisible(visible)
        s = self.size()
        s.setHeight(1) # with this, the height will be adapted to fit the visible widgets
        self.resize(s)
    def fillChildTags(self):
        d = SelectTagDialog(self._path, self)
        d.tagLabel.setText("&Tag to use")
        tag = d.run()
        if not tag is None:
            self.childTagsModel.setFromParentTag(str(tag))
