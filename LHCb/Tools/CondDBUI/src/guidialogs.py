import qt, qttable
import os
import guiextras

#=============================================#
#                EDITVALUEDIALOG              #
#=============================================#
class editValueDialog(qt.QDialog):
    '''
    A very simple dialog which allows to edit the value of an LHCb
    Condition parameter when it is a vector of elements. The generated
    string will be put "as is" in the xml representation of the LHCb
    Condition.
    '''
    def __init__(self, parent, name = 'editValueDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        self.callingRow = -1
        
        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutExit   = qt.QHBoxLayout(5)

        #--- Editor ---#
        self.editValue = qt.QTextEdit(self, 'editValue')
        self.editValue.setTextFormat(qt.Qt.PlainText)
        self.editValue.setFont(qt.QFont("Courier", 10))

        #--- Exit ---#
        self.buttonOK = qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog window layout ---#
        self.layoutDialog.addWidget(self.editValue)
        self.layoutDialog.addLayout(self.layoutExit)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.cancel)

    def cancel(self):
        '''
        Reset and close the dialog window
        '''
        self.reset()
        self.hide()
        
    def reset(self):
        '''
        Reset the widget
        '''
        self.callingRow = -1
        self.editValue.setText('')

#=============================================#
#             CREATELHCBCONDDIALOG            #
#=============================================#
class createLHCbCondDialog(qt.QDialog):
    '''
    Opens a dialog allowing to set the type, name and value(s) of a list of
    parameters defining an LHCb condition. The result is an xml string to
    include in a Condition Object.
    '''
    def __init__(self, parent, name = 'createLHCbCondDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        self.xmlString = ''
        self.listTypes = qt.QStringList.fromStrList(['Integer', 'Double', 'String', 'Vector of Integers', 'Vector of Doubles'])

        #--- Layout ---#
        self.layoutDialog = qt.QGridLayout(self, 3, 2, 5, -1, 'layoutDialog')
        self.layoutCondInfo = qt.QHBoxLayout()
        self.layoutExit   = qt.QHBoxLayout()

        #--- Other dialog ---#
        self.dialogEditValue = editValueDialog(self, 'dialogEditValue')
        
        #--- Condition ---#
        self.labelCondName = qt.QLabel('Condition Name:', self, 'labelCondName')
        self.editCondName  = qt.QLineEdit(self, 'editCondName')
        self.labelClassID = qt.QLabel('Class ID:', self, 'labelClassID')
        self.editClassID  = qt.QLineEdit('5', self, 'editClassID')

        #--- Parameters ---#
        self.buttonMovePad = guiextras.movePad(self, 'movePad', ['Move Up', 'Move Down', 'Del', 'Add'])
        self.tableParams = qttable.QTable(0, 3, self, 'tableParams')
        self.tableParams.setColumnLabels(qt.QStringList.fromStrList(['Parameter Name', 'Parameter Type', 'Parameter Value']))
        for i in range(self.tableParams.numCols()):
            self.tableParams.adjustColumn(i)
            
        #--- Exit ---#
        self.buttonOK = qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog window layout ---#
        self.layoutDialog.addMultiCellLayout(self.layoutCondInfo, 0, 0, 0, 1)
        self.layoutCondInfo.addWidget(self.labelCondName)
        self.layoutCondInfo.addWidget(self.editCondName)
        self.layoutCondInfo.addWidget(self.labelClassID)
        self.layoutCondInfo.addWidget(self.editClassID)
        
        self.layoutDialog.addWidget(self.buttonMovePad, 1, 0)
        self.layoutDialog.addWidget(self.tableParams, 1, 1)
        self.layoutDialog.addMultiCellLayout(self.layoutExit, 2, 2, 0, 1)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonMovePad.buttonUp,  qt.SIGNAL("clicked()"), self.moveUp)
        self.connect(self.buttonMovePad.buttonDown,qt.SIGNAL("clicked()"), self.moveDown)
        self.connect(self.buttonMovePad.buttonLeft, qt.SIGNAL("clicked()"), self.deleteParam)
        self.connect(self.buttonMovePad.buttonRight, qt.SIGNAL("clicked()"), self.addParam)
        self.connect(self.tableParams,  qt.SIGNAL("doubleClicked(int, int, int, const QPoint &)"), self.openValueEditor)
        self.connect(self.tableParams,  qt.SIGNAL("valueChanged(int, int)"), self.updateValueDisplay)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.cancel)
        self.connect(self.dialogEditValue.buttonOK, qt.SIGNAL("clicked()"), self.setVectorValue)

    def cancel(self):
        '''
        Reset and close the dialog window
        '''
        self.reset()
        self.hide()

    def reset(self):
        '''
        Reset everything to initial values.
        '''
        self.xmlString = ''
        self.editCondName.setText('')
        self.editClassID.setText('5')
        self.tableParams.setNumRows(0)
        
    def deleteParam(self):
        '''
        Remove a parameter from the table
        '''
        self.tableParams.removeRow(self.tableParams.currentRow())
        

    def moveUp(self):
        '''
        Move the selected parameter up in the table
        '''
        row = self.tableParams.currentRow()
        if row > 0:
            self.tableParams.swapRows(row, row - 1)
            self.tableParams.updateContents()
            
    def moveDown(self):
        '''
        Move the selected parameter down in the table
        '''
        row = self.tableParams.currentRow()
        if row < self.tableParams.numRows() - 1:
            self.tableParams.swapRows(row, row + 1)
            self.tableParams.updateContents()
            
    def addParam(self):
        '''
        Add a new parameter to the table
        '''
        comboListTypes = qttable.QComboTableItem(self.tableParams, self.listTypes)
        paramValue     = guiextras.memoryTableItem(self.tableParams)
        newRow = self.tableParams.numRows()
        self.tableParams.insertRows(newRow)
        self.tableParams.setItem(newRow, 1, comboListTypes)
        self.tableParams.setItem(newRow, 2, paramValue)

    def updateValueDisplay(self, row, column):
        '''
        Check if the change in cell value implies a modification of the parameter
        value display.
        '''
        if column == 1:
            # the parameter type has been modified
            newText = str(self.tableParams.item(row, 1).currentText())
            paramValue = self.tableParams.item(row, 2)
            if 'Vector' in newText:
                paramValue.setText('Double-Click to Display')
            else:
                paramValue.setEnabled(True)
                paramValue.setText(str(paramValue.textMemory))
        elif column == 2:
            newText = str(self.tableParams.item(row, 2).text())
            self.tableParams.item(row, 2).setTextMemory(newText)
            
        for i in range(self.tableParams.numCols()):
            self.tableParams.adjustColumn(i)
    
    def openValueEditor(self, row, column, mousePos):
        '''
        Check if it is necessary to open the value editor, depending on the
        type of the parameter.
        '''
        if column == 2 and 'Vector' in str(self.tableParams.item(row, 1).currentText()):
            self.dialogEditValue.callingRow = row
            self.dialogEditValue.editValue.setText(self.tableParams.item(row, 2).textMemory)
            self.dialogEditValue.show()
        else:
            pass

    def setVectorValue(self):
        '''
        Get a vector value from the value editor dialog and store it
        '''
        self.tableParams.item(self.dialogEditValue.callingRow, 2).setTextMemory(self.dialogEditValue.editValue.text())
        self.dialogEditValue.hide()

    def createXml(self):
        '''
        builds the xml string to be written in the CondDB and returns it
        '''
        conditionName = str(self.editCondName.text())
        if conditionName == '':
            errorMsg = qt.QMessageBox('conddbui.py',\
                                      "The condition's name is empty.\nPlease give a name to this condition",\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return ''

        try:
            classID = int(str(self.editClassID.text()))
        except:
            errorMsg = qt.QMessageBox('conddbui.py',\
                                      "The class ID value is not valid.\nPlease give a correct class ID",\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return ''

        self.xmlString = '<condition classID="%s" name="%s">\n'%(classID, conditionName)
        for i in range(self.tableParams.numRows()):
            paramName  = str(self.tableParams.text(i, 0))
            paramType  = str(self.tableParams.text(i, 1))
            paramValue = str(self.tableParams.item(i, 2).textMemory)

            if paramName == '':
                errorMsg = qt.QMessageBox('conddbui.py',\
                                      "The parameter's name is empty.\nPlease give a name to this parameter",\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return ''
            
            if 'Integer' in paramType:
                valueType = 'int'
            elif 'Double' in paramType:
                valueType = 'double'
            elif 'String' in paramType:
                valueType = 'string'
            else:
                valueType = 'unknown'

            if 'Vector' in paramType:
                self.xmlString += '<paramVector name="%s" type="%s">\n%s\n</paramVector>\n'%(paramName, valueType, paramValue)
            else:
                self.xmlString += '<param name="%s" type="%s">%s</param>\n'%(paramName, valueType, paramValue)

        self.xmlString += '</condition>'

        return self.xmlString


#=============================================#
#               CREATETAGDIALOG               #
#=============================================#
class createTagDialog(qt.QDialog):
    '''
    This dialog allows to assign a new tag to a conddb node.
    '''
    def __init__(self, parent, name = 'createTagDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        self.selectedTags = []

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutEdit   = qt.QGridLayout(self.layoutDialog, 2, 2)
        self.layoutChild  = qt.QVBoxLayout(self.layoutDialog)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- Folder ---#
        self.labelNode = qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = qt.QLineEdit(self, 'editNode')
        self.editNode.setReadOnly(True)

        #--- Tag ---#
        self.labelTag = qt.QLabel('Tag Name:', self, 'labelTag')
        self.editTag  = qt.QLineEdit(self, 'editTag')

        #--- Child Table ---#
        self.labelChild = qt.QLabel('Child Nodes version selection table:', self, 'labelChild')
        self.tableChild = qttable.QTable(0, 2, self, 'tableChild')
        self.tableChild.setColumnLabels(qt.QStringList.fromStrList(['Node Path', 'Tag Name']))
        self.tableChild.setColumnReadOnly(0, True)

        #--- Exit ---#
        self.buttonOK     = qt.QPushButton('Create', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

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
        self.connect(self.buttonOK,     qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.reject)


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
                tagList = qt.QStringList()
                for tag in bridge.getTagList(child):
                    tagList.append(tag.name)
                comboItem = qttable.QComboTableItem(self.tableChild, tagList)
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
        return qt.QDialog.accept(self)

    def reject(self):
        self.reset()
        return qt.QDialog.reject(self)


#=============================================#
#               DELETETAGDIALOG               #
#=============================================#
class deleteTagDialog(qt.QDialog):
    '''
    This dialog allows to delete a tag associated with
    a node.
    '''
    def __init__(self, parent, name = 'deleteTagDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutEdit   = qt.QGridLayout(self.layoutDialog, 2, 2)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- Node ---#
        self.labelNode = qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = qt.QLineEdit(self, 'editNode')
        self.editNode.setReadOnly(True)

        #--- Tag ---#
        self.labelTag = qt.QLabel('Tag Name:', self, 'labelTag')
        self.choseTag = qt.QComboBox(self, 'choseTag')

        #--- Exit ---#
        self.buttonOK     = qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutEdit.addWidget(self.labelNode, 0, 0)
        self.layoutEdit.addWidget(self.editNode,  0, 1)
        self.layoutEdit.addWidget(self.labelTag,  1, 0)
        self.layoutEdit.addWidget(self.choseTag,  1, 1)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.buttonOK,       qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,   qt.SIGNAL("clicked()"), self.reject)

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
                self.choseTag.insertItem(tag.name)

    def accept(self):
        return qt.QDialog.accept(self)

    def reject(self):
        self.editNode.setText('')
        self.choseTag.clear()
        return qt.QDialog.reject(self)


#=============================================#
#               CREATENODEDIALOG              #
#=============================================#
class createNodeDialog(qt.QDialog):
    '''
    This dialog allows to create a new folder or a new folderset in the database
    '''
    def __init__(self, parent, name = 'createFolderDialog'):
        '''
        initialisation of the dialog window.
        '''
        qt.QDialog.__init__(self, parent, name)

        self.nodeName   = ''
        self.is_folderset = False
        self.description  = ''
        self.is_singleVersion = False
        self.createParents = True
        self.storageKeys = []

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)

        #--- Node info ---#
        self.groupInfo = qt.QHGroupBox('Node Info', self)
        self.layoutDialog.addWidget(self.groupInfo)

        self.layoutInfo = qt.QGrid(2, self.groupInfo)
        self.labelNode = qt.QLabel('Node Name:', self.layoutInfo, 'labelNode')
        self.editNode  = qt.QLineEdit(self.layoutInfo, 'editNode')
        self.labelDescription = qt.QLabel('Description:', self.layoutInfo, 'labelDescription')
        self.editDescription = qt.QLineEdit(self.layoutInfo, 'editDescription')

        #--- Storage key ---#
        self.groupStorage = qt.QVGroupBox('Storage Keys', self)
        self.layoutDialog.addWidget(self.groupStorage)

        # list
        self.tableKeys = qttable.QTable(1, 1, self.groupStorage)
        self.tableKeys.horizontalHeader().setLabel(0, 'Key Name')
        self.tableKeys.setText(0, 0, 'data')

        # buttons
        self.layoutButton = qt.QHBox(self.groupStorage)
        self.buttonAdd = qt.QPushButton('Add', self.layoutButton)
        self.buttonDel = qt.QPushButton('Del', self.layoutButton)

        #--- Check boxes ---#
        self.groupOptions = qt.QVGroupBox('Options', self)
        self.layoutDialog.addWidget(self.groupOptions)

        self.checkFolderset = qt.QCheckBox('Folderset', self.groupOptions, 'checkFolderset')
        self.checkSingleVersion = qt.QCheckBox('Single Version', self.groupOptions, 'checkSingleVersion')
        self.checkCreateParents = qt.QCheckBox('Create parents', self.groupOptions, 'checkCreateParents')
        self.checkCreateParents.setChecked(True)


        #--- Exit buttons ---#
        self.layoutExit = qt.QHBox(self)
        self.layoutDialog.addWidget(self.layoutExit)

        self.buttonCreate = qt.QPushButton('Create', self.layoutExit, 'buttonCreate')
        self.buttonCancel = qt.QPushButton('Cancel', self.layoutExit, 'buttonCancel')

        #--- Signals connection ---#
        self.connect(self.buttonAdd,    qt.SIGNAL("clicked()"), self.addKey)
        self.connect(self.buttonDel,    qt.SIGNAL("clicked()"), self.delKey)
        self.connect(self.buttonCreate, qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.reject)


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
        return qt.QDialog.accept(self)


    def reject(self):
        self.reset()
        return qt.QDialog.reject(self)


#=============================================#
#               DELETENODEDIALOG              #
#=============================================#
class deleteNodeDialog(qt.QDialog):
    '''
    This dialog allows to delete an existing folder or a folderset from the database
    '''
    def __init__(self, parent, name = 'deleteNodeDialog'):
        '''
        initialisation of the dialog window.
        '''
        qt.QDialog.__init__(self, parent, name)

        self.folderName   = ''

        #--- Layout ---#
        self.layoutDialog  = qt.QVBoxLayout(self)
        self.layoutWarning = qt.QHBoxLayout(self.layoutDialog)
        self.layoutNode    = qt.QHBoxLayout(self.layoutDialog)
        self.layoutExit    = qt.QHBoxLayout(self.layoutDialog)

        #--- Folder info ---#
        self.labelWarning = qt.QLabel('WARNING: removal can not be undone !!', self, 'labelWarning')
        self.labelNode = qt.QLabel('Node name:', self, 'labelNode')
        self.editNode  = qt.QLineEdit(self, 'editFolder')

        #--- Exit buttons ---#
        self.buttonDelete = qt.QPushButton('Delete', self, 'buttonDelete')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutWarning.addWidget(self.labelWarning)

        self.layoutNode.addWidget(self.labelNode)
        self.layoutNode.addWidget(self.editNode)

        self.layoutExit.addWidget(self.buttonDelete)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonDelete,     qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,     qt.SIGNAL("clicked()"), self.reject)

    def reset(self, defaultNodeName = ''):
        '''
        reset the dialog
        '''
        self.nodeName = defaultNodeName
        self.editNode.setText(defaultNodeName)

    def accept(self):
        self.nodeName = str(self.editNode.text())
        return qt.QDialog.accept(self)

    def reject(self):
        self.reset()
        return qt.QDialog.reject(self)

#=============================================#
#              ADDCONDITIONDIALOG             #
#=============================================#

class addConditionDialog(qt.QDialog):
    '''
    This dialog allows to create a serie of condition objects and add
    them to the selected folder of the CondDB. A condition object "template"
    can be loaded from one or more xml files (e.g. from the XmlCondition
    package) and modified. Be aware however that no xml validity checking
    is performed !
    '''
    def __init__(self, parent, name = 'addConditionDialog'):
        '''
        initialisation of the dialog window.
        '''
        qt.QDialog.__init__(self, parent, name)

        # The address of the folder containing the new condition objects
        self.folderName = ''
        # The list of objects to be added in the folder.
        # It will contain dictionaries of 5 elements: path, channelID, since, until, payload
        self.objectList = []
        self.activeObject = None
        self.activePayload = {}

        #--- Main Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self, 5, -1, 'layoutDialog')
        #-------------------#

        #--- Condition ObjectLocation ---#
        self.groupDetails = qt.QHGroupBox('Condition Object Details', self, 'groupDetails')
        self.layoutDialog.addWidget(self.groupDetails)

        # Location in the Database
        self.groupLocation = qt.QHGroupBox('Location', self.groupDetails, 'groupLocation')
        self.layoutLocation = qt.QGrid(2, self.groupLocation, 'layoutLocation')
        self.layoutLocation.setSpacing(5)

        self.timeValidator = guiextras.valKeyValidator(self, 'timeValidator')
        self.channelValidator = qt.QIntValidator(self, 'channelValidator')
        self.channelValidator.setBottom(0)

        self.labelFolder = qt.QLabel('Folder: ', self.layoutLocation, 'labelFolder')
        self.editFolder  = qt.QLineEdit(self.layoutLocation, 'editFolder')
        self.editFolder.setReadOnly(True)

        self.labelChannelID = qt.QLabel('ChannelID: ', self.layoutLocation, 'labelChannelID')
        self.editChannelID  = qt.QLineEdit('0', self.layoutLocation, 'editChannelID')
        self.editChannelID.setValidator(self.channelValidator)
        self.editChannelID.setAlignment(qt.Qt.AlignRight)

        self.labelSince = qt.QLabel('Since: ', self.layoutLocation, 'labelSince')
        self.editSince  = qt.QLineEdit(str(self.timeValidator.valKeyMin), self.layoutLocation, 'editSince')
        self.editSince.setValidator(self.timeValidator)
        self.editSince.setAlignment(qt.Qt.AlignRight)

        self.labelUntil = qt.QLabel('Until: ', self.layoutLocation, 'labelUntil')
        self.editUntil  = qt.QLineEdit(str(self.timeValidator.valKeyMax), self.layoutLocation, 'editUntil')
        self.editUntil.setValidator(self.timeValidator)
        self.editUntil.setAlignment(qt.Qt.AlignRight)

        # Payload list
        self.groupPayload = qt.QVGroupBox('Payload List', self.groupDetails, 'groupPayload')

        self.selectPayload = qt.QListBox(self.groupPayload, 'selectPayload')
        self.selectPayload.setSelectionMode(qt.QListBox.Extended)

        self.buttonEditItem = qt.QPushButton('Edit', self.groupPayload, 'buttonEditItem')
        #--------------------------------#

        #--- Condition Objects Stack ---#
        self.layoutStack = qt.QHBox(self, 'layoutStack')
        self.layoutDialog.addWidget(self.layoutStack)

        # Stack table
        self.groupStack = qt.QHGroupBox('Condition Objects Stack', self.layoutStack, 'groupStack')

        self.movePad = guiextras.movePad(self.groupStack, 'movePad', ['Move Up', 'Move Down', 'Del', 'Add'])

        self.tableCondObjects = qttable.QTable(0, 0, self.groupStack, 'tableCondObjects')
        self.columnLabels = [('path', 'Path'), ('channel', 'ChannelID'), ('since', 'Since'), ('until', 'Until')]
        for col in self.columnLabels:
            i = self.tableCondObjects.numCols()
            self.tableCondObjects.insertColumns(i, 1)
            self.tableCondObjects.horizontalHeader().setLabel(i, col[1])
            self.tableCondObjects.adjustColumn(i)
        self.tableCondObjects.setReadOnly(True)
        #-------------------------------#

        #--- Exit buttons ---#
        self.layoutExit = qt.QHBox(self, 'layoutExit')
        self.layoutDialog.addWidget(self.layoutExit)

        self.buttonWrite = qt.QPushButton('Write', self.layoutExit, 'buttonWrite')
        self.buttonCancel = qt.QPushButton('Cancel', self.layoutExit, 'buttonCancel')
        #--------------------#

        #--- Signals connection ---#
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.cancel)
        # Connection to self.buttonWrite "clicked" signal is done in the
        # guiwin.myWindow class.

        self.connect(self.buttonEditItem,   qt.SIGNAL("clicked()"), self.editPayloadKeys)

        self.connect(self.tableCondObjects, qt.SIGNAL("currentChanged(int, int)"), self.reloadObject)
        self.connect(self.tableCondObjects, qt.SIGNAL("selectionChanged()"),       self.reloadObject)

        self.connect(self.movePad.buttonRight, qt.SIGNAL("clicked()"), self.addObject)
        self.connect(self.movePad.buttonLeft,  qt.SIGNAL("clicked()"), self.removeObject)
        self.connect(self.movePad.buttonUp,    qt.SIGNAL("clicked()"), self.moveObjectUp)
        self.connect(self.movePad.buttonDown,  qt.SIGNAL("clicked()"), self.moveObjectDown)
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


    def _fillPayloadKeys(self):
        '''
        Fill the Payload key list
        '''
        self.selectPayload.clear()
        keyList = self.currentPayload.keys()
        keyList.sort()
        for k in keyList:
            self.selectPayload.insertItem(k)


    def editPayloadKeys(self):
        '''
        Open a text editor to modify the selected payload keys.
        '''
        payloadSelected = {}
        for i in range(self.selectPayload.count()):
            if self.selectPayload.isSelected(i):
                key = str(self.selectPayload.text(i))
                payloadSelected[key] = self.currentPayload[key]

        if payloadSelected:
            xmlEditor = conditionEditorDialog(payloadSelected, self)
            if xmlEditor.exec_loop():
                payload = xmlEditor.getPayload()
                for k in payload.keys():
                    self.currentPayload[k] = payload[k]


    def reset(self, defaultFolder = '/', xmlDict = {}, defaultChannelID = 0):
        '''
        Reset everything to initial values.
        '''
        self.objectList = []
        self.currentPayload = xmlDict.copy()
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
            newObject['since']   = long(str(self.editSince.text()))
            newObject['until']   = long(str(self.editUntil.text()))
            newObject['path']    = str(self.editFolder.text())
            newObject['payload'] = self.currentPayload.copy()
        except:
            errorMsg = qt.QMessageBox('conddbui.py',\
                                      'At least one field is empty\nPlease give all the necessary information to create a new object.',\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
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

class conditionEditorDialog(qt.QDialog):

    def __init__(self, payload, parent, name = 'conditionEditorDialog'):
        qt.QDialog.__init__(self, parent, name)

        self.layoutDialog = qt.QVBoxLayout(self, 0, 10)

        self.tabEditors = qt.QTabWidget(self, 'tabEditors')
        self.layoutDialog.addWidget(self.tabEditors)

        self.layoutButtons = qt.QHBox(self, 'layoutButtons')
        self.layoutDialog.addWidget(self.layoutButtons)
        self.buttonOK = qt.QPushButton('OK', self.layoutButtons)
        self.buttonCancel = qt.QPushButton('Cancel', self.layoutButtons)

        self.connect(self.buttonOK, qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.reject)

        self.setPayload(payload)

    def setPayload(self, payload):
        for key in payload.keys():
            page = guiextras.ConditionEditor(self)
            page.setEditorText(payload[key])
            self.tabEditors.addTab(page, key)

    def getPayload(self):
        payload = {}
        for i in range(self.tabEditors.count()):
            page = self.tabEditors.page(i)
            key = str(self.tabEditors.tabLabel(page))
            payload[key] = str(page.getEditorText())
        return payload



#=============================================#
#             CONDDBCONNECTDIALOG             #
#=============================================#

class condDBConnectDialog(qt.QDialog):
    '''
    This dialog allows to chose the DBLookup alias and database name of
    a CondDB to open. If allowed, the user can also chose the access mode.
    '''
    def __init__(self, parent, name = 'condDBConnectDialog'):
        '''
        Initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        # The string used to connect to the condDB
        self.connectString = ''

        #--- Layout ---#
        self.layoutDialog = qt.QGridLayout(self, 3, 3, 5, -1, 'layoutDialog')

        #--- dbLookup aliases ---#
        self.labelAlias = qt.QLabel('DBLookup Alias: ', self, 'labelAlias')
        self.choseAlias = qt.QComboBox(self, 'choseAlias')
        self.choseAlias.setEditable(True)
        self.choseAlias.setAutoCompletion(True)
        self.choseAlias.setInsertionPolicy(qt.QComboBox.NoInsertion)

        self.aliasDict = guiextras.readDBLookup()
        self.choseAlias.insertStringList(qt.QStringList.fromStrList(self.aliasDict.keys()))
        self.choseAlias.listBox().sort(True)
        self.choseAlias.insertItem('SQLite file')

        #--- Database Name ---#
        self.labelDBName = qt.QLabel('Database Name: ', self, 'labelDBName')
        self.editDBName  = qt.QLineEdit(self, 'editDBName')

        #--- Action Buttons ---#
        # Display locked/unlocked status of the database
        self.buttonLocked   = qt.QPushButton('Read Only',   self, 'buttonLocked')
        self.buttonLocked.setToggleButton(True)
        self.buttonLocked.setOn(True)
        self.buttonLocked.setFlat(True)
        # connect to the DB and exit
        self.buttonOpenDB = qt.QPushButton('Open DB', self, 'buttonOpenDB')
        # exit whithout doing anything
        self.buttonCancel = qt.QPushButton('Cancel',  self, 'buttonCancel')
        self.layoutButton = qt.QHBoxLayout()

        #--- Dialog Layout ---#
        self.layoutDialog.addWidget(self.labelAlias, 0, 0)
        self.layoutDialog.addWidget(self.choseAlias, 0, 1)
        self.layoutDialog.addWidget(self.buttonLocked, 0, 2)

        self.layoutDialog.addWidget(self.labelDBName, 1, 0)
        self.layoutDialog.addWidget(self.editDBName,  1, 1)

        self.layoutDialog.addMultiCellLayout(self.layoutButton, 2, 2, 0, 2)
        self.layoutButton.addWidget(self.buttonOpenDB)
        self.layoutButton.addWidget(self.buttonCancel)

        #--- Signals and slots connections ---#
        self.connect(self.choseAlias, qt.SIGNAL("activated(const QString &)"), self.aliasChanged)
        self.connect(self.buttonLocked, qt.SIGNAL("stateChanged(int)"), self.lockStatusChanged)
        self.connect(self.buttonOpenDB, qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.reject)

        #--- Initialise the session ---#
        # load the first of the saved sessions
        self.choseAlias.setCurrentItem(0)
        self.choseAlias.emit(qt.SIGNAL("activated"),(self.choseAlias.currentText(),))

    def aliasChanged(self, newAliasName):
        '''
        Action taken when a new alias is selected from the choseAlias QComboBox.
        This will check the access status of the database
        '''
        # As the alias can be edited by the user, one need to check if it already
        # exists or not.
        if self.choseAlias.listBox().findItem(newAliasName, qt.Qt.ExactMatch):
            alias = str(newAliasName)
            self.buttonLocked.setOn(True)
            if alias == 'SQLite file':
                self.buttonLocked.setDisabled(False)
                fileDialog = qt.QFileDialog(self, 'fileDialog', True)
                if fileDialog.exec_loop():
                    self.choseAlias.setCurrentText(fileDialog.selectedFile())
            else:
                if self.aliasDict[alias] != 'update':
                    self.buttonLocked.setDisabled(True)
                else:
                    self.buttonLocked.setDisabled(False)

    def lockStatusChanged(self, status):
        '''
        Change the lock button display when its status is modified
        '''
        if status == qt.QButton.On:
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
        if alias.find(os.sep) != -1:
            self.connectString = 'sqlite://none;schema=%s;dbname=%s'%(alias, dbname)
        else:
            self.connectString = '%s/%s'%(alias, dbname)
        return qt.QDialog.accept(self)

    def reject(self):
        '''
        Dialog cancelation function.
        '''
        return qt.QDialog.reject(self)


#=============================================#
#              CREATECONDDBDIALOG             #
#=============================================#

class createCondDBDialog(qt.QDialog):
    '''
    Dialog allowing to select the filename and database name
    of a new CondDB with SQLite backend.
    '''
    def __init__(self, parent, name = 'createCondDBDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        self.connectionString = ''

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutEdit   = qt.QGridLayout(self.layoutDialog, 2, 3)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- file name ---#
        self.labelSchema  = qt.QLabel('SQLite file name:', self, 'labelSchema')
        self.editSchema   = qt.QLineEdit(self, 'editSchema')
        self.buttonSchema = qt.QPushButton('...', self, 'buttonSchema')
        self.buttonSchema.setMaximumWidth(30)
        self.fileDialogSchema = qt.QFileDialog(self, 'fileDialogSchema', True)
        self.fileDialogSchema.setMode(qt.QFileDialog.AnyFile)

        #--- Database Name ---#
        self.labelDBName = qt.QLabel('Database name:', self, 'labelDBName')
        self.editDBName  = qt.QLineEdit(self, 'editDBName')

        #--- Exit ---#
        self.buttonOK     = qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutEdit.addWidget(self.labelSchema,  0, 0)
        self.layoutEdit.addWidget(self.editSchema,   0, 1)
        self.layoutEdit.addWidget(self.buttonSchema, 0, 2)
        self.layoutEdit.addWidget(self.labelDBName,  1, 0)
        self.layoutEdit.addWidget(self.editDBName,   1, 1)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.buttonSchema, qt.SIGNAL("clicked()"), self.schemaSelect)
        self.connect(self.buttonOK,     qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"), self.reject)

    def schemaSelect(self):
        '''
        set the schema name using the file dialog.
        '''
        if self.fileDialogSchema.exec_loop():
            self.editSchema.setText(self.fileDialogSchema.selectedFile())

    def accept(self):
        '''
        Set the values of the schema and dbnames.
        '''
        self.connectionString = 'sqlite://; schema=%s; dbname=%s;'%(str(self.editSchema.text()), str(self.editDBName.text()))
        self.editSchema.setText('')
        self.editDBName.setText('')
        return qt.QDialog.accept(self)

    def reject(self):
        '''
        Dialog cancelation function.
        '''
        self.connectionString = ''
        self.editSchema.setText('')
        self.editDBName.setText('')
        return qt.QDialog.reject(self)

#================================================#
