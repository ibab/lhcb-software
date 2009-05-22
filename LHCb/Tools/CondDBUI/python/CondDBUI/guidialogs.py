from PyQt4 import Qt, Qt3Support
import os
import guiextras
import CondDBUI

#=======================================#
#             SLICINGDIALOG             #
#=======================================#

class slicingDialog(Qt.QDialog):
    '''
    This dialog allows to select a list of data from the current
    conddb and copy it to a new or existing database.
    This is a limited graphical interface to the PyCoolCopy module.
    '''
    def __init__(self, parent, name = 'slicingDialog'):
        '''
        initialisation of the dialog window.
        '''
        Qt.QDialog.__init__(self, parent, name)

        # The address of the folder containing the new condition objects
        self.folderName = ''
        # The list of selection objects to be copied to the target database
        # It will contain dictionaries of 4 elements: path, tag, since, until.
        self.objectList = []
        # Flag to decide if we copy data to a new database or if we put data
        # on top of an existing one
        self.do_copy = True

        #--- Main Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self, 5, -1, 'layoutDialog')
        #-------------------#

        #--- Target database ---#
        self.groupTarget = Qt3Support.Q3HGroupBox('Target Database', self, 'groupTarget')
        self.layoutDialog.addWidget(self.groupTarget)

        self.layoutTarget = Qt3Support.Q3Grid(3, self.groupTarget, 'layoutLocation')
        self.layoutTarget.setSpacing(5)

        self.labelSchema  = Qt.QLabel('SQLite file name:', self.layoutTarget, 'labelSchema')
        self.editSchema   = Qt.QLineEdit(self.layoutTarget, 'editSchema')
        self.buttonSchema = Qt.QPushButton('...', self.layoutTarget, 'buttonSchema')
        self.buttonSchema.setMaximumWidth(30)

        self.labelDBName = Qt.QLabel('Database name:', self.layoutTarget, 'labelDBName')
        self.editDBName  = Qt.QLineEdit(self.layoutTarget, 'editDBName')
        #-----------------------#

        #--- Selection Objects ---#
        self.groupSelection = Qt3Support.Q3VGroupBox('Selection Object Creation', self, 'groupSelection')
        self.layoutDialog.addWidget(self.groupSelection)

        # Selection details
        self.layoutDetails = Qt3Support.Q3Grid(2, self.groupSelection, 'layoutDetails')
        self.layoutDetails.setSpacing(5)

        # Database Node
        self.labelNode = Qt.QLabel('Node: ', self.layoutDetails, 'labelNode')
        self.choseNode = Qt3Support.Q3ComboBox(self.layoutDetails, 'choseNode')
        self.choseNode.setInsertionPolicy(Qt3Support.Q3ComboBox.NoInsertion)
        self.choseNode.setEditable(True)
        self.choseNode.setAutoCompletion(True)
        self.choseNode.setAutoResize(True)

        # Coordinates (time and tag)
        self.timeValidator = guiextras.valKeyValidator(self)

        self.labelSince = Qt.QLabel('Since: ', self.layoutDetails, 'labelSince')
        self.editSince  = Qt.QLineEdit(str(self.timeValidator.valKeyMin), self.layoutDetails, 'editSince')
        self.editSince.setValidator(self.timeValidator)
        self.editSince.setAlignment(Qt.Qt.AlignRight)

        self.labelUntil = Qt.QLabel('Until: ', self.layoutDetails, 'labelUntil')
        self.editUntil  = Qt.QLineEdit(str(self.timeValidator.valKeyMax), self.layoutDetails, 'editUntil')
        self.editUntil.setValidator(self.timeValidator)
        self.editUntil.setAlignment(Qt.Qt.AlignRight)

        self.labelTag  = Qt.QLabel('Tag Name: ', self.layoutDetails, 'labelTag')
        self.choseTag  = Qt3Support.Q3ListBox(self.layoutDetails, 'choseTag')
        self.choseTag.setSelectionMode(Qt3Support.Q3ListBox.Extended)
        #--------------------------------#

        #--- Condition Objects Stack ---#
        self.layoutStack = Qt3Support.Q3HBox(self, 'layoutStack')
        self.layoutDialog.addWidget(self.layoutStack)

        # Stack table
        self.groupStack = Qt3Support.Q3HGroupBox('Selection Objects List', self.layoutStack, 'groupStack')

        self.tableSelectObjects = Qt3Support.Q3Table(0, 0, self.groupStack, 'tableCondObjects')
        self.columnLabels = [('path', 'Path'), ('tag', 'Tag Name'), ('since', 'Since'), ('until', 'Until')]
        for col in self.columnLabels:
            i = self.tableSelectObjects.numCols()
            self.tableSelectObjects.insertColumns(i, 1)
            self.tableSelectObjects.horizontalHeader().setLabel(i, col[1])
            self.tableSelectObjects.adjustColumn(i)
        self.tableSelectObjects.setReadOnly(True)
        
        self.movePad = guiextras.movePad(self.groupStack, 'movePad', ['Move Up', 'Move Down', 'Del', 'Add'])
        #-------------------------------#

        #--- Exit buttons ---#
        self.layoutExit = Qt3Support.Q3HBox(self, 'layoutExit')
        self.layoutDialog.addWidget(self.layoutExit)

        self.buttonCopy   = Qt.QPushButton('Copy', self.layoutExit, 'buttonCopy')
        self.buttonAppend = Qt.QPushButton('Append', self.layoutExit, 'buttonAppend')
        self.buttonCancel = Qt.QPushButton('Cancel', self.layoutExit, 'buttonCancel')
        #--------------------#

        #--- Signals connection ---#
        self.connect(self.buttonCancel,        Qt.SIGNAL("clicked()"), self.cancel)
        self.connect(self.buttonCopy,          Qt.SIGNAL("clicked()"), self.acceptCopy)
        self.connect(self.buttonAppend,        Qt.SIGNAL("clicked()"), self.acceptAppend)
        self.connect(self.choseNode,           Qt.SIGNAL("textChanged(const QString &)"), self.loadTags)
        self.connect(self.buttonSchema,        Qt.SIGNAL("clicked()"), self.schemaSelect)
        self.connect(self.movePad.buttonLeft,  Qt.SIGNAL("clicked()"), self.addObject)
        self.connect(self.movePad.buttonRight, Qt.SIGNAL("clicked()"), self.removeObject)
        self.connect(self.movePad.buttonUp,    Qt.SIGNAL("clicked()"), self.moveObjectUp)
        self.connect(self.movePad.buttonDown,  Qt.SIGNAL("clicked()"), self.moveObjectDown)
        #--------------------------#


    def _fillTable(self):
        '''
        Fill the table with the contents of the object list.
        '''
        self.tableSelectObjects.setNumRows(0)
        nbLines = 0
        nbCols  = self.tableSelectObjects.numCols()
        for obj in self.objectList:
            self.tableSelectObjects.insertRows(nbLines,1)
            for i in range(nbCols):
                self.tableSelectObjects.setText(nbLines, i, str(obj[self.columnLabels[i][0]]))
            nbLines += 1
        for i in range(self.tableSelectObjects.numCols()):
            self.tableSelectObjects.adjustColumn(i)

    def loadNodes(self):
        '''
        Load all the nodes in the choseNode combo box
        '''
        nodeList = self.parent().bridge.getAllNodes()
        self.choseNode.insertStringList(nodeList)
        self.choseNode.setCurrentItem(0)
        self.choseNode.emit(Qt.SIGNAL("textChanged"), (self.choseNode.currentText(),))

    def loadTags(self, nodeName):
        '''
        Loads the tags related to the given node
        '''
        if nodeName:
            tagList = self.parent().bridge.getTagList(str(nodeName))
            self.choseTag.clear()
            if len(tagList) <= 1:
                self.choseTag.insertItem('HEAD')
            else:
                for tag in tagList:
                    nbItems = self.choseTag.count()
                    ancestors = tag.getAncestors()
                    if tag.name.find('_auto_') == -1:
                        self.choseTag.insertItem(tag.name)
                    if tag.name == 'HEAD':
                        self.defaultTagIndex = self.choseTag.count() - 1
                    else:
                        for a in ancestors:
                            if a.find('_auto_') == -1:
                                self.choseTag.insertItem(a)
                    if nbItems < self.choseTag.count():
                        self.choseTag.insertItem('-' * 20)
            self.choseTag.setCurrentItem(0)

    def schemaSelect(self):
        '''
        set the schema name using the file dialog.
        '''
        fileDialogSchema = Qt.QFileDialog(self, "Select SQLite file")
        fileDialogSchema.setFileMode(Qt.QFileDialog.AnyFile)
        if fileDialogSchema.exec_() and fileDialogSchema.selectedFiles():
            self.editSchema.setText(fileDialogSchema.selectedFiles()[0])

    def reset(self):
        '''
        Reset everything to initial values.
        '''
        self.editSchema.setText('')
        self.editDBName.setText('')
        self.objectList = []
        self.tableSelectObjects.setNumRows(0)
        self.choseNode.clear()
        self.editSince.setText(str(self.timeValidator.valKeyMin))
        self.editUntil.setText(str(self.timeValidator.valKeyMax))

    def removeObject(self):
        '''
        Remove an object form the list
        '''
        objectIndex = self.tableSelectObjects.currentRow()
        self.objectList.pop(objectIndex)
        self._fillTable()

    def moveObjectUp(self):
        '''
        Move the selected object up in the list
        '''
        objectIndex = self.tableSelectObjects.currentRow()
        colIndex    = self.tableSelectObjects.currentColumn()
        if objectIndex > 0:
            self.objectList.insert(objectIndex-1, self.objectList.pop(objectIndex))
            self._fillTable()
            self.tableSelectObjects.setCurrentCell(objectIndex - 1, colIndex)

    def moveObjectDown(self):
        '''
        Move the selected object down in the list
        '''
        objectIndex = self.tableSelectObjects.currentRow()
        colIndex    = self.tableSelectObjects.currentColumn()
        if objectIndex < len(self.objectList) - 1:
            self.objectList.insert(objectIndex+1, self.objectList.pop(objectIndex))
            self._fillTable()


    def addObject(self):
        '''
        Add a new object to the list of condition objects to write
        '''
        newObject = {}
        try:
            newObject['since'] = CondDBUI.ValidityKeyWrapper(str(self.editSince.text()))
            newObject['until'] = CondDBUI.ValidityKeyWrapper(str(self.editUntil.text()))
            newObject['path']  = str(self.choseNode.currentText())
            newObject['tag']   = []
            for i in range(self.choseTag.count()):
                if self.choseTag.isSelected(i):
                    item = self.choseTag.item(i)
                    if str(item.text()).find('-----') == -1:
                        newObject['tag'].append(str(item.text()))
        except:
            errorMsg = Qt.QMessageBox('CondDBUI',\
                                      'At least one field is not correct\nPlease give all the necessary information to create a new object.',\
                                      Qt.QMessageBox.Warning,\
                                      Qt.QMessageBox.Ok,\
                                      Qt.QMessageBox.NoButton,\
                                      Qt.QMessageBox.NoButton)
            errorMsg.exec_()
        else:
            self.objectList.append(newObject)
            self._fillTable()

    def acceptCopy(self):
        '''
        Set the do_copy flag to True and exit
        '''
        self.do_copy = True
        self.accept()

    def acceptAppend(self):
        '''
        Set the do_copy flag to False and exit
        '''
        self.do_copy = False
        self.accept()

    def cancel(self):
        '''
        Reset the values and hide the dialog window
        '''
        self.reset()
        self.hide()


#=============================================#
#               CREATETAGDIALOG               #
#=============================================#
class createTagDialog(Qt.QDialog):
    '''
    This dialog allows to assign a new tag to a conddb node.
    '''
    def __init__(self, parent, name = 'createTagDialog'):
        '''
        initialisation of the dialog window
        '''
        Qt.QDialog.__init__(self, parent, name)

        self.selectedTags = []

        #--- Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self)
        self.layoutEdit   = Qt.QGridLayout(self.layoutDialog, 2, 2)
        self.layoutChild  = Qt.QVBoxLayout(self.layoutDialog)
        self.layoutExit   = Qt.QHBoxLayout(self.layoutDialog)

        #--- Folder ---#
        self.labelNode = Qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = Qt.QLineEdit(self, 'editNode')
        self.editNode.setReadOnly(True)

        #--- Tag ---#
        self.labelTag = Qt.QLabel('Tag Name:', self, 'labelTag')
        self.editTag  = Qt.QLineEdit(self, 'editTag')

        #--- Child Table ---#
        self.labelChild = Qt.QLabel('Child Nodes version selection table:', self, 'labelChild')
        self.tableChild = Qt3Support.Q3Table(0, 2, self, 'tableChild')
        self.tableChild.setColumnLabels(['Node Path', 'Tag Name'])
        self.tableChild.setColumnReadOnly(0, True)

        #--- Exit ---#
        self.buttonOK     = Qt.QPushButton('Create', self, 'buttonOK')
        self.buttonCancel = Qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutEdit.addWidget(self.labelNode, 0, 0)
        self.layoutEdit.addWidget(self.editNode,  0, 1)
        self.layoutEdit.addWidget(self.labelTag,  1, 0)
        self.layoutEdit.addWidget(self.editTag,   1, 1)
        self.layoutChild.addWidget(self.labelChild)
        self.layoutChild.addWidget(self.tableChild)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.buttonOK,     Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.reject)


    def fillTable(self):
        '''
        Get the list of child from the node and fill the child table.
        '''
        bridge = self.parent().bridge
        nodeName = str(self.editNode.text())
        if bridge.db.existsFolder(nodeName):
            pass
        else:
            self.labelChild.show()
            self.tableChild.show()
            childList = bridge.getChildNodes(nodeName)
            self.tableChild.setNumRows(len(childList))
            for i in range(len(childList)):
                child = childList[i]
                self.tableChild.setText(i, 0, child)
                tagList = Qt.QStringList()
                for tag in bridge.getTagList(child):
                    tagList.append(tag.name)
                comboItem = Qt3Support.Q3ComboTableItem(self.tableChild, tagList)
                self.tableChild.setItem(i, 1, comboItem)

    def reset(self):
        self.tableChild.setNumRows(0)
        self.tableChild.hide()
        self.labelChild.hide()
        self.selectedTags = []
        self.editNode.setText('')
        self.editTag.setText('')


    def accept(self):
        for i in range(self.tableChild.numRows()):
            tagInfo = [str(self.tableChild.text(i,0)), str(self.tableChild.item(i,1).currentText())]
            self.selectedTags.append(tagInfo)
        return Qt.QDialog.accept(self)

    def reject(self):
        self.reset()
        return Qt.QDialog.reject(self)


#=============================================#
#               DELETETAGDIALOG               #
#=============================================#
class deleteTagDialog(Qt.QDialog):
    '''
    This dialog allows to delete a tag associated with
    a node.
    '''
    def __init__(self, parent, name = 'deleteTagDialog'):
        '''
        initialisation of the dialog window
        '''
        Qt.QDialog.__init__(self, parent, name)

        #--- Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self)
        self.layoutEdit   = Qt.QGridLayout(self.layoutDialog, 2, 2)
        self.layoutExit   = Qt.QHBoxLayout(self.layoutDialog)

        #--- Node ---#
        self.labelNode = Qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = Qt.QLineEdit(self, 'editNode')
        self.editNode.setReadOnly(True)

        #--- Tag ---#
        self.labelTag = Qt.QLabel('Tag Name:', self, 'labelTag')
        self.choseTag = Qt3Support.Q3ComboBox(self, 'choseTag')
        self.choseTag.setAutoResize(True)

        #--- Exit ---#
        self.buttonOK     = Qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = Qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutEdit.addWidget(self.labelNode, 0, 0)
        self.layoutEdit.addWidget(self.editNode,  0, 1)
        self.layoutEdit.addWidget(self.labelTag,  1, 0)
        self.layoutEdit.addWidget(self.choseTag,  1, 1)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.buttonOK,       Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,   Qt.SIGNAL("clicked()"), self.reject)

    def reloadTags(self):
        '''
        fill the tag list corresponding to the chosen node.
        '''
        # retrieve the bridge from the main window
        bridge = self.parent().bridge
        nodeName = str(self.editNode.text())

        # just in case...
        self.choseTag.clear()
        if nodeName <> '':
            tagList = bridge.getTagList(nodeName)
            for tag in tagList:
                if tag.name != 'HEAD':
                    self.choseTag.insertItem(tag.name)

    def accept(self):
        return Qt.QDialog.accept(self)

    def reject(self):
        self.editNode.setText('')
        self.choseTag.clear()
        return Qt.QDialog.reject(self)


#=============================================#
#               CREATENODEDIALOG              #
#=============================================#
class createNodeDialog(Qt.QDialog):
    '''
    This dialog allows to create a new folder or a new folderset in the database
    '''
    def __init__(self, parent, name = 'createFolderDialog'):
        '''
        initialisation of the dialog window.
        '''
        Qt.QDialog.__init__(self, parent, name)

        self.nodeName   = ''
        self.is_folderset = False
        self.description  = ''
        self.is_singleVersion = False
        self.createParents = True
        self.storageKeys = []

        #--- Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self)

        #--- Node info ---#
        self.groupInfo = Qt3Support.Q3HGroupBox('Node Info', self)
        self.layoutDialog.addWidget(self.groupInfo)

        self.layoutInfo = Qt3Support.Q3Grid(2, self.groupInfo)
        self.labelNode = Qt.QLabel('Node Name:', self.layoutInfo, 'labelNode')
        self.editNode  = Qt.QLineEdit(self.layoutInfo, 'editNode')
        self.labelDescription = Qt.QLabel('Description:', self.layoutInfo, 'labelDescription')
        self.editDescription = Qt.QLineEdit(self.layoutInfo, 'editDescription')

        #--- Storage key ---#
        self.groupStorage = Qt3Support.Q3VGroupBox('Storage Keys', self)
        self.layoutDialog.addWidget(self.groupStorage)

        # list
        self.tableKeys = Qt3Support.Q3Table(1, 1, self.groupStorage)
        self.tableKeys.horizontalHeader().setLabel(0, 'Key Name')
        self.tableKeys.setText(0, 0, 'data')

        # buttons
        self.layoutButton = Qt3Support.Q3HBox(self.groupStorage)
        self.buttonAdd = Qt.QPushButton('Add', self.layoutButton)
        self.buttonDel = Qt.QPushButton('Del', self.layoutButton)

        #--- Check boxes ---#
        self.groupOptions = Qt3Support.Q3VGroupBox('Options', self)
        self.layoutDialog.addWidget(self.groupOptions)

        self.checkFolderset = Qt.QCheckBox('Folderset', self.groupOptions, 'checkFolderset')
        self.checkSingleVersion = Qt.QCheckBox('Single Version', self.groupOptions, 'checkSingleVersion')
        self.checkCreateParents = Qt.QCheckBox('Create parents', self.groupOptions, 'checkCreateParents')
        self.checkCreateParents.setChecked(True)


        #--- Exit buttons ---#
        self.layoutExit = Qt3Support.Q3HBox(self)
        self.layoutDialog.addWidget(self.layoutExit)

        self.buttonCreate = Qt.QPushButton('Create', self.layoutExit, 'buttonCreate')
        self.buttonCancel = Qt.QPushButton('Cancel', self.layoutExit, 'buttonCancel')

        #--- Signals connection ---#
        self.connect(self.buttonAdd,    Qt.SIGNAL("clicked()"), self.addKey)
        self.connect(self.buttonDel,    Qt.SIGNAL("clicked()"), self.delKey)
        self.connect(self.buttonCreate, Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.reject)


    def reset(self):
        '''
        reset the dialog
        '''
        self.nodeName = ''
        self.editNode.setText('')
        self.description = ''
        self.editDescription.setText('')
        self.is_folderset = False
        self.checkFolderset.setChecked(False)
        self.is_singleVersion = False
        self.checkSingleVersion.setChecked(False)
        self.createParents = True
        self.checkCreateParents.setChecked(True)
        self.tableKeys.setNumRows(1)
        self.tableKeys.setText(0, 0, 'data')
        self.storageKeys = []


    def addKey(self):
        '''
        Add a new key to the table
        '''
        self.tableKeys.insertRows(self.tableKeys.numRows())


    def delKey(self):
        '''
        Remove the active item
        '''
        if self.tableKeys.numRows() > 1:
            self.tableKeys.removeRow(self.tableKeys.currentRow())


    def accept(self):
        self.nodeName = str(self.editNode.text())
        self.description = str(self.editDescription.text())
        self.is_folderset = self.checkFolderset.isChecked()
        self.is_singleVersion = self.checkSingleVersion.isChecked()
        self.createParents = self.checkCreateParents.isChecked()
        for i in range(self.tableKeys.numRows()):
            text = str(self.tableKeys.text(i, 0))
            if text != '':
                self.storageKeys.append(text)
        return Qt.QDialog.accept(self)


    def reject(self):
        self.reset()
        return Qt.QDialog.reject(self)


#=============================================#
#               DELETENODEDIALOG              #
#=============================================#
class deleteNodeDialog(Qt.QDialog):
    '''
    This dialog allows to delete an existing folder or a folderset from the database
    '''
    def __init__(self, parent, name = 'deleteNodeDialog'):
        '''
        initialisation of the dialog window.
        '''
        Qt.QDialog.__init__(self, parent, name)

        self.folderName   = ''

        #--- Layout ---#
        self.layoutDialog  = Qt.QVBoxLayout(self)
        self.layoutWarning = Qt.QHBoxLayout(self.layoutDialog)
        self.layoutNode    = Qt.QHBoxLayout(self.layoutDialog)
        self.layoutExit    = Qt.QHBoxLayout(self.layoutDialog)

        #--- Folder info ---#
        self.labelWarning = Qt.QLabel('WARNING: removal can not be undone !!', self, 'labelWarning')
        self.labelNode = Qt.QLabel('Node name:', self, 'labelNode')
        self.editNode  = Qt.QLineEdit(self, 'editFolder')

        #--- Exit buttons ---#
        self.buttonDelete = Qt.QPushButton('Delete', self, 'buttonDelete')
        self.buttonCancel = Qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutWarning.addWidget(self.labelWarning)

        self.layoutNode.addWidget(self.labelNode)
        self.layoutNode.addWidget(self.editNode)

        self.layoutExit.addWidget(self.buttonDelete)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonDelete,     Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,     Qt.SIGNAL("clicked()"), self.reject)

    def reset(self, defaultNodeName = ''):
        '''
        reset the dialog
        '''
        self.nodeName = defaultNodeName
        self.editNode.setText(defaultNodeName)

    def accept(self):
        self.nodeName = str(self.editNode.text())
        return Qt.QDialog.accept(self)

    def reject(self):
        self.reset()
        return Qt.QDialog.reject(self)

#=============================================#
#              ADDCONDITIONDIALOG             #
#=============================================#

class addConditionDialog(Qt.QDialog):
    '''
    This dialog allows to create a serie of condition objects and add
    them to the selected folder of the CondDB. A condition object "template"
    can be loaded from one or more xml files (e.g. from the XmlCondition
    package) and modified. Be aware however that no xml validity checking
    is performed !
    '''
    def __init__(self, parent, name = 'addConditionDialog', externalEditorCmd = ''):
        '''
        initialisation of the dialog window.
        '''
        Qt.QDialog.__init__(self, parent, name)

        # The address of the folder containing the new condition objects
        self.folderName = ''
        # The list of objects to be added in the folder.
        # It will contain dictionaries of 5 elements: path, channelID, since, until, payload
        self.objectList = []
        self.activeObject = None
        self.activePayload = {}

        self.xmlEditor = conditionEditorDialog(self, extension = '', externalEditorCmd = externalEditorCmd)
        
        #--- Main Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self, 5, -1, 'layoutDialog')
        #-------------------#

        #--- Condition ObjectLocation ---#
        self.groupDetails = Qt3Support.Q3HGroupBox('Condition Object Details', self, 'groupDetails')
        self.layoutDialog.addWidget(self.groupDetails)

        # Location in the Database
        self.groupLocation = Qt3Support.Q3HGroupBox('Location', self.groupDetails, 'groupLocation')
        self.layoutLocation = Qt3Support.Q3Grid(2, self.groupLocation, 'layoutLocation')
        self.layoutLocation.setSpacing(5)

        self.timeValidator = guiextras.valKeyValidator(self)
        self.channelValidator = Qt.QIntValidator(self)
        self.channelValidator.setBottom(0)

        self.labelFolder = Qt.QLabel('Folder: ', self.layoutLocation, 'labelFolder')
        self.editFolder  = Qt.QLineEdit(self.layoutLocation, 'editFolder')
        self.editFolder.setReadOnly(True)

        self.labelChannelID = Qt.QLabel('ChannelID: ', self.layoutLocation, 'labelChannelID')
        self.editChannelID  = Qt.QLineEdit('0', self.layoutLocation, 'editChannelID')
        self.editChannelID.setValidator(self.channelValidator)
        self.editChannelID.setAlignment(Qt.Qt.AlignRight)

        self.labelSince = Qt.QLabel('Since: ', self.layoutLocation, 'labelSince')
        self.editSince  = Qt.QLineEdit(str(self.timeValidator.valKeyMin), self.layoutLocation, 'editSince')
        self.editSince.setValidator(self.timeValidator)
        self.editSince.setAlignment(Qt.Qt.AlignRight)

        self.labelUntil = Qt.QLabel('Until: ', self.layoutLocation, 'labelUntil')
        self.editUntil  = Qt.QLineEdit(str(self.timeValidator.valKeyMax), self.layoutLocation, 'editUntil')
        self.editUntil.setValidator(self.timeValidator)
        self.editUntil.setAlignment(Qt.Qt.AlignRight)

        # Payload list
        self.groupPayload = Qt3Support.Q3VGroupBox('Payload Keys', self.groupDetails, 'groupPayload')

        self.selectPayload = Qt3Support.Q3ListBox(self.groupPayload, 'selectPayload')
        self.selectPayload.setSelectionMode(Qt3Support.Q3ListBox.Extended)

        self.buttonEditItem = Qt.QPushButton('Edit', self.groupPayload, 'buttonEditItem')
        #--------------------------------#

        #--- Condition Objects Stack ---#
        self.layoutStack = Qt3Support.Q3HBox(self, 'layoutStack')
        self.layoutDialog.addWidget(self.layoutStack)

        # Stack table
        self.groupStack = Qt3Support.Q3HGroupBox('Condition Objects Stack', self.layoutStack, 'groupStack')

        self.tableCondObjects = Qt3Support.Q3Table(0, 0, self.groupStack, 'tableCondObjects')
        self.columnLabels = [('path', 'Path'), ('channel', 'ChannelID'), ('since', 'Since'), ('until', 'Until')]
        for col in self.columnLabels:
            i = self.tableCondObjects.numCols()
            self.tableCondObjects.insertColumns(i, 1)
            self.tableCondObjects.horizontalHeader().setLabel(i, col[1])
            self.tableCondObjects.adjustColumn(i)
        self.tableCondObjects.setReadOnly(True)
        
        self.movePad = guiextras.movePad(self.groupStack, 'movePad', ['Move Up', 'Move Down', 'Del', 'Add'])
        #-------------------------------#

        #--- Exit buttons ---#
        self.layoutExit = Qt3Support.Q3HBox(self, 'layoutExit')
        self.layoutDialog.addWidget(self.layoutExit)

        self.buttonWrite = Qt.QPushButton('Write', self.layoutExit, 'buttonWrite')
        self.buttonCancel = Qt.QPushButton('Cancel', self.layoutExit, 'buttonCancel')
        #--------------------#

        #--- Signals connection ---#
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.cancel)
        # Connection to self.buttonWrite "clicked" signal is done in the
        # guiwin.myWindow class.

        self.connect(self.buttonEditItem,   Qt.SIGNAL("clicked()"), self.editPayloadKeys)

        self.connect(self.tableCondObjects, Qt.SIGNAL("currentChanged(int, int)"), self.reloadObject)
        self.connect(self.tableCondObjects, Qt.SIGNAL("selectionChanged()"),       self.reloadObject)

        self.connect(self.movePad.buttonLeft,  Qt.SIGNAL("clicked()"), self.addObject)
        self.connect(self.movePad.buttonRight, Qt.SIGNAL("clicked()"), self.removeObject)
        self.connect(self.movePad.buttonUp,    Qt.SIGNAL("clicked()"), self.moveObjectUp)
        self.connect(self.movePad.buttonDown,  Qt.SIGNAL("clicked()"), self.moveObjectDown)
        #--------------------------#

    def _fillTable(self):
        '''
        Fill the table with the contents of the object list.
        '''
        self.tableCondObjects.setNumRows(0)
        nbLines = 0
        nbCols  = self.tableCondObjects.numCols()
        for obj in self.objectList:
            self.tableCondObjects.insertRows(nbLines,1)
            for i in range(nbCols):
                self.tableCondObjects.setText(nbLines, i, str(obj[self.columnLabels[i][0]]))
            nbLines += 1
        for i in range(self.tableCondObjects.numCols()):
            self.tableCondObjects.adjustColumn(i)
        # select the last item
        if nbLines > 0:
            self.tableCondObjects.setCurrentCell(nbLines - 1, 0)


    def _fillPayloadKeys(self):
        '''
        Fill the Payload key list
        '''
        self.selectPayload.clear()
        keyList = self.activePayload.keys()
        keyList.sort()
        for k in keyList:
            self.selectPayload.insertItem(k)
        # select the first item
        # if self.selectPayload.count() > 0:
        #     self.selectPayload.setCurrentItem(self.selectPayload.count() - 1)
        for i in range( self.selectPayload.count() ):
            self.selectPayload.setSelected(i,True)


    def editPayloadKeys(self):
        '''
        Open a text editor to modify the selected payload keys.
        '''
        payloadSelected = {}
        for i in range(self.selectPayload.count()):
            if self.selectPayload.isSelected(i):
                key = str(self.selectPayload.text(i))
                payloadSelected[key] = self.activePayload[key]

        if payloadSelected:
            self.xmlEditor.reset()
            self.xmlEditor.setPayload(payloadSelected)
            if self.xmlEditor.exec_():
                payload = self.xmlEditor.getPayload()
                for k in payload.keys():
                    self.activePayload[k] = payload[k]


    def reset(self, defaultFolder = '/', xmlDict = {}, defaultChannelID = 0, externalEditorCmd = ''):
        '''
        Reset everything to initial values.
        '''
        self.xmlEditor.extension = os.path.splitext(defaultFolder)[1]
        self.xmlEditor.externalEditorCmd = externalEditorCmd
        self.objectList = []
        self.activePayload = xmlDict.copy()
        self.selectPayload.clear()
        self.setFolderName(defaultFolder)
        self.setDefaultChannelID(defaultChannelID)
        self.setIoV(self.timeValidator.valKeyMin, self.timeValidator.valKeyMax)
        self._fillTable()
        self._fillPayloadKeys()
        
    def removeObject(self):
        '''
        Remove an object form the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        self.objectList.pop(objectIndex)
        self.selectPayload.clear()
        self._fillTable()
        self._fillPayloadKeys()


    def moveObjectUp(self):
        '''
        Move the selected object up in the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        colIndex    = self.tableCondObjects.currentColumn()
        if objectIndex > 0:
            self.objectList.insert(objectIndex-1, self.objectList.pop(objectIndex))
            self._fillTable()
            self.tableCondObjects.setCurrentCell(objectIndex - 1, colIndex)


    def moveObjectDown(self):
        '''
        Move the selected object down in the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        colIndex    = self.tableCondObjects.currentColumn()
        if objectIndex < len(self.objectList) - 1:
            self.objectList.insert(objectIndex+1, self.objectList.pop(objectIndex))
            self._fillTable()


    def reloadObject(self, row = -1, col = -1):
        '''
        Fill the parameters with the informations from the object selected from the table.
        '''
        if self.objectList:
            if row == -1:
                row = self.tableCondObjects.currentRow()
            self.activeObject = self.objectList[row]
            self.activePayload = self.activeObject['payload'].copy()
            self.editSince.setText(str(self.activeObject['since']))
            self.editUntil.setText(str(self.activeObject['until']))
            self.editChannelID.setText(str(self.activeObject['channel']))
            self.editFolder.setText(str(self.activeObject['path']))


    def addObject(self):
        '''
        Add a new object to the list of condition objects to write
        '''
        newObject = {}
        try:
            newObject['channel'] = int(str(self.editChannelID.text()))
            newObject['since']   = CondDBUI.ValidityKeyWrapper(str(self.editSince.text()))
            newObject['until']   = CondDBUI.ValidityKeyWrapper(str(self.editUntil.text()))
            newObject['path']    = str(self.editFolder.text())
            newObject['payload'] = self.activePayload.copy()
        except:
            errorMsg = Qt.QMessageBox('CondDBUI',\
                                      'At least one field is not correct\nPlease give all the necessary information to create a new object.',\
                                      Qt.QMessageBox.Warning,\
                                      Qt.QMessageBox.Ok,\
                                      Qt.QMessageBox.NoButton,\
                                      Qt.QMessageBox.NoButton)
            errorMsg.exec_()
        else:
            self.objectList.append(newObject)
            self.activeObject = self.objectList[-1]
            self._fillTable()


    def setFolderName(self, folderName):
        '''
        Sets the full path to the folder where we want to put new condition objects.
        NB: the path must exist !
        '''
        self.folderName = folderName
        self.editFolder.setText(self.folderName)


    def setDefaultChannelID(self, ID):
        '''
        Set the channel ID default value to the given one
        '''
        self.editChannelID.setText(str(ID))


    def setIoV(self, since, until):
        '''
        Sets the editSince and editUntil values.
        '''
        self.editSince.setText(str(since))
        self.editUntil.setText(str(until))


    def cancel(self):
        '''
        Reset the values and hide the dialog window
        '''
        self.reset()
        self.hide()

#=============================================#
#            CONDITIONEDITORDIALOG            #
#=============================================#

class conditionEditorDialog(Qt.QDialog):

    def __init__(self, parent, name = 'conditionEditorDialog', extension = '', externalEditorCmd = ''):
        Qt.QDialog.__init__(self, parent, name)

        self.layoutDialog = Qt.QVBoxLayout(self, 0, 10)

        self.tabEditors = Qt.QTabWidget(self, 'tabEditors')
        self.layoutDialog.addWidget(self.tabEditors)

        self.layoutButtons = Qt3Support.Q3HBox(self, 'layoutButtons')
        self.layoutDialog.addWidget(self.layoutButtons)
        self.buttonOK = Qt.QPushButton('OK', self.layoutButtons)
        self.buttonCancel = Qt.QPushButton('Cancel', self.layoutButtons)

        self.connect(self.buttonOK, Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.reject)
        
        self.extension = extension
        self.externalEditorCmd = externalEditorCmd

    def reset(self):
        while self.tabEditors.count() > 0:
            i = self.tabEditors.currentIndex()
            page = self.tabEditors.widget(i)
            self.tabEditors.removeTab(i)
            del(page)

    def setPayload(self, payload):
        keys = payload.keys()
        keys.sort()
        for key in keys:
            page = guiextras.ConditionEditor(self,
                                             extension=self.extension,
                                             externalEditorCmd=self.externalEditorCmd)
            page.setEditorText(payload[key])
            self.tabEditors.addTab(page, key)

    def getPayload(self):
        payload = {}
        for i in range(self.tabEditors.count()):
            page = self.tabEditors.widget(i)
            key = str(self.tabEditors.tabText(i))
            payload[key] = str(page.getEditorText())
        return payload



#=============================================#
#             CONDDBCONNECTDIALOG             #
#=============================================#

class condDBConnectDialog(Qt.QDialog):
    '''
    This dialog allows to chose the DBLookup alias and database name of
    a CondDB to open. If allowed, the user can also chose the access mode.
    '''
    def __init__(self, parent, name = 'condDBConnectDialog'):
        '''
        Initialisation of the dialog window
        '''
        Qt.QDialog.__init__(self, parent, name)

        # The string used to connect to the condDB
        self.connectString = ''

        #--- Layout ---#
        self.layoutDialog = Qt.QGridLayout(self, 3, 3, 5, -1, 'layoutDialog')

        #--- dbLookup aliases ---#
        self.labelAlias = Qt.QLabel('DBLookup Alias: ', self, 'labelAlias')
        self.choseAlias = Qt3Support.Q3ComboBox(self, 'choseAlias')
        self.choseAlias.setEditable(True)
        self.choseAlias.setAutoCompletion(True)
        self.choseAlias.setInsertionPolicy(Qt3Support.Q3ComboBox.NoInsertion)
        self.choseAlias.setAutoResize(True)

        self.aliasDict = guiextras.readDBLookup()
        self.choseAlias.insertStringList(self.aliasDict.keys())
        #self.choseAlias.listBox().sort(True)
        self.choseAlias.insertItem('SQLite file')

        #--- Database Name ---#
        self.labelDBName = Qt.QLabel('Database Name: ', self, 'labelDBName')
        self.editDBName  = Qt.QLineEdit(self, 'editDBName')
        #self.choseDBName = Qt3Support.Q3ComboBox(self, 'editDBName')
        #self.choseDBName.setEditable(True)
        #self.choseDBName.setAutoCompletion(True)

        #--- Action Buttons ---#
        # Display locked/unlocked status of the database
        self.buttonLocked = Qt.QPushButton('Read Only', self, 'buttonLocked')
        self.buttonLocked.setCheckable(True)
        self.buttonLocked.setChecked(True)
        self.buttonLocked.setFlat(True)
        # connect to the DB and exit
        self.buttonOpenDB = Qt.QPushButton('Open DB', self, 'buttonOpenDB')
        # exit whithout doing anything
        self.buttonCancel = Qt.QPushButton('Cancel',  self, 'buttonCancel')
        self.layoutButton = Qt.QHBoxLayout()

        #--- Dialog Layout ---#
        self.layoutDialog.addWidget(self.labelAlias, 0, 0)
        self.layoutDialog.addWidget(self.choseAlias, 0, 1)
        self.layoutDialog.addWidget(self.buttonLocked, 0, 2)

        self.layoutDialog.addWidget(self.labelDBName, 1, 0)
        self.layoutDialog.addWidget(self.editDBName,  1, 1)
        #self.layoutDialog.addWidget(self.choseDBName,  1, 1)

        self.layoutDialog.addItem(self.layoutButton, 2, 0, 2, 2)
        self.layoutButton.addWidget(self.buttonOpenDB)
        self.layoutButton.addWidget(self.buttonCancel)

        #--- Signals and slots connections ---#
        self.connect(self.choseAlias, Qt.SIGNAL("activated(const QString &)"), self.aliasChanged)
        self.connect(self.buttonLocked, Qt.SIGNAL("toggled(bool)"), self.lockStatusChanged)
        self.connect(self.buttonOpenDB, Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.reject)

        #--- Initialise the session ---#
        # load the first of the saved sessions
        #self.choseAlias.setCurrentItem(0)
        #self.choseAlias.emit(Qt.SIGNAL("activated"),(self.choseAlias.currentText(),))

    def aliasChanged(self, newAliasName):
        '''
        Action taken when a new alias is selected from the choseAlias QComboBox.
        This will check the access status of the database
        '''
        # As the alias can be edited by the user, one need to check if it already
        # exists or not.
        found = False
        for i in range(self.choseAlias.count()):
            if self.choseAlias.text(i) == newAliasName:
                found = True
                break
        if found:
            alias = str(newAliasName)
            self.buttonLocked.setChecked(True)
            if alias == 'SQLite file':
                self.buttonLocked.setDisabled(False)
                filename = Qt.QFileDialog.getOpenFileName(self)
                if filename:
                    self.choseAlias.setCurrentText(filename)
            else:
                if self.aliasDict[alias] != 'update':
                    self.buttonLocked.setDisabled(True)
                else:
                    self.buttonLocked.setDisabled(False)

    def lockStatusChanged(self, checked):
        '''
        Change the lock button display when its status is modified
        '''
        if checked:
            self.buttonLocked.setText('Read Only')
        else:
            self.buttonLocked.setText('Read/Write')

    def accept(self):
        '''
        Dialog acceptation function. It creates the connectString attribute
        given the current connection parameters.
        '''
        alias  = str(self.choseAlias.currentText())
        dbname = str(self.editDBName.text())
        #dbname = str(self.choseDBName.currentText())
        # FIXME: this check does not work always (CORAL alias can have a '/')
        if alias.find(os.sep) != -1:
            self.connectString = 'sqlite_file:%s/%s'%(alias, dbname)
        else:
            self.connectString = '%s/%s'%(alias, dbname)
        return Qt.QDialog.accept(self)

    def reject(self):
        '''
        Dialog cancelation function.
        '''
        return Qt.QDialog.reject(self)


#=============================================#
#              CREATECONDDBDIALOG             #
#=============================================#

class createCondDBDialog(Qt.QDialog):
    '''
    Dialog allowing to select the filename and database name
    of a new CondDB with SQLite backend.
    '''
    def __init__(self, parent, name = 'createCondDBDialog'):
        '''
        initialisation of the dialog window
        '''
        Qt.QDialog.__init__(self, parent, name)

        self.connectionString = ''

        #--- Layout ---#
        self.layoutDialog = Qt.QVBoxLayout(self)
        self.layoutEdit   = Qt.QGridLayout(self.layoutDialog, 2, 3)
        self.layoutExit   = Qt.QHBoxLayout(self.layoutDialog)

        #--- file name ---#
        self.labelSchema  = Qt.QLabel('SQLite file name:', self, 'labelSchema')
        self.editSchema   = Qt.QLineEdit(self, 'editSchema')
        self.buttonSchema = Qt.QPushButton('...', self, 'buttonSchema')
        self.buttonSchema.setMaximumWidth(30)

        #--- Database Name ---#
        self.labelDBName = Qt.QLabel('Database name:', self, 'labelDBName')
        self.editDBName  = Qt.QLineEdit(self, 'editDBName')

        #--- Exit ---#
        self.buttonOK     = Qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = Qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutEdit.addWidget(self.labelSchema,  0, 0)
        self.layoutEdit.addWidget(self.editSchema,   0, 1)
        self.layoutEdit.addWidget(self.buttonSchema, 0, 2)
        self.layoutEdit.addWidget(self.labelDBName,  1, 0)
        self.layoutEdit.addWidget(self.editDBName,   1, 1)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.buttonSchema, Qt.SIGNAL("clicked()"), self.schemaSelect)
        self.connect(self.buttonOK,     Qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, Qt.SIGNAL("clicked()"), self.reject)

    def schemaSelect(self):
        '''
        set the schema name using the file dialog.
        '''
        fileDialogSchema = Qt.QFileDialog(self, "Select SQLite file")
        fileDialogSchema.setFileMode(Qt.QFileDialog.AnyFile)
        if fileDialogSchema.exec_() and fileDialogSchema.selectedFiles():
            self.editSchema.setText(fileDialogSchema.selectedFiles()[0])

    def accept(self):
        '''
        Set the values of the schema and dbnames.
        '''
        self.connectionString = 'sqlite_file:%s/%s'%(str(self.editSchema.text()), str(self.editDBName.text()))
        self.editSchema.setText('')
        self.editDBName.setText('')
        return Qt.QDialog.accept(self)

    def reject(self):
        '''
        Dialog cancelation function.
        '''
        self.connectionString = ''
        self.editSchema.setText('')
        self.editDBName.setText('')
        return Qt.QDialog.reject(self)

#================================================#
