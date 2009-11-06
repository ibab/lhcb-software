## @file MainWindow.py
#  Class for the main window.
#
#  Definition of the browser MainWindow class.

from PyQt4.QtCore import (Qt, QObject, SIGNAL,
                          QRegExp)
from PyQt4.QtGui import (QDialog,
                         QFileDialog,
                         QDialogButtonBox,
                         QValidator,
                         QRegExpValidator)

from Models import NodeFieldsModel, NodeFieldsDelegate

from Ui_NewDatabaseDialog import Ui_NewDatabaseDialog
from Ui_OpenDatabaseDialog import Ui_OpenDatabaseDialog
from Ui_NewNodeDialog import Ui_NewNodeDialog

import os

__all__ = ["NewDatabaseDialog", "OpenDatabaseDialog", "NewNodeDialog"]

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

## Class containing the logic of the application.
class NewDatabaseDialog(QDialog, Ui_NewDatabaseDialog):
    ## Constructor.
    #  Initializes the base class, define some internal structures and set the icon of
    #  the window.
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


## Class containing the logic of the application.
class OpenDatabaseDialog(QDialog, Ui_OpenDatabaseDialog):
    ## Constructor.
    #  Initializes the base class, define some internal structures and set the icon of
    #  the window.
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
        print "removeField"
        if self.fieldsView.selectionModel().hasSelection():
            index = self.fieldsView.selectionModel().currentIndex()
            self.fieldsModel.removeRow(index.row())
