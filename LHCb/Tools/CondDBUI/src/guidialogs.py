import qt, qttable
import shelve
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
        self.layoutNode   = qt.QHBoxLayout(self.layoutDialog)
        self.layoutTag    = qt.QHBoxLayout(self.layoutDialog)
        self.layoutChild  = qt.QVBoxLayout(self.layoutDialog)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- Folder ---#
        self.labelNode = qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = qt.QLineEdit(self, 'editNode')
        self.editNode.setReadOnly(True)

        #--- Tag ---#
        self.labelTag    = qt.QLabel('Tag Name:', self, 'labelTag')
        self.editTag     = qt.QLineEdit(self, 'editTag')

        #--- Child Table ---#
        self.labelChild = qt.QLabel('Child Nodes version selection table:', self, 'labelChild')
        self.tableChild = qttable.QTable(0, 2, self, 'tableChild')
        self.tableChild.setColumnLabels(qt.QStringList.fromStrList(['Node Path', 'Tag Name']))
        self.tableChild.setColumnReadOnly(0, True)

        #--- Exit ---#
        self.buttonOK     = qt.QPushButton('Create', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutNode.addWidget(self.labelNode)
        self.layoutNode.addWidget(self.editNode)
        self.layoutTag.addWidget(self.labelTag)
        self.layoutTag.addWidget(self.editTag)
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
    a folder or to delete a global tag (in the LHCb sens)
    '''
    def __init__(self, parent, name = 'deleteTagDialog'):
        '''
        initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutTag    = qt.QHBoxLayout(self.layoutDialog)
        self.layoutFolder = qt.QHBoxLayout(self.layoutDialog)
        self.layoutCheck  = qt.QHBoxLayout(self.layoutDialog)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- Tag ---#
        self.labelTag = qt.QLabel('Tag Name:', self, 'labelTag')
        self.choseTag = qt.QComboBox(self, 'choseTag')

        #--- Folder ---#
        self.labelFolder = qt.QLabel('Folder Name:', self, 'labelFolder')
        self.editFolder  = qt.QLineEdit(self, 'editFolder')
        self.editFolder.setEnabled(False)

        #--- Check box ---#
        self.checkBoxGlobal = qt.QCheckBox('LHCb Global Tags (can be slow !)', self, 'checkBoxGlobal')
        self.checkBoxGlobal.setChecked(True)

        #--- Exit ---#
        self.buttonOK     = qt.QPushButton('OK', self, 'buttonOK')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutTag.addWidget(self.labelTag)
        self.layoutTag.addWidget(self.choseTag)
        self.layoutFolder.addWidget(self.labelFolder)
        self.layoutFolder.addWidget(self.editFolder)
        self.layoutCheck.addWidget(self.checkBoxGlobal)
        self.layoutExit.addWidget(self.buttonOK)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- signal connections ---#
        self.connect(self.checkBoxGlobal, qt.SIGNAL("clicked()"), self.toggleFolder)
        self.connect(self.editFolder,     qt.SIGNAL("lostFocus()"), self.reloadTags)
        self.connect(self.buttonOK,       qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,   qt.SIGNAL("clicked()"), self.reject)

    def toggleFolder(self):
        '''
        Toggle on/off the possibility to change the folder name
        '''
        if self.checkBoxGlobal.isChecked():
            self.editFolder.setEnabled(False)
        else:
            self.editFolder.setEnabled(True)
        self.reloadTags()

    def reloadTags(self):
        '''
        fill the tag list corresponding to the chosen folder. If the
        global check box is checked, then the list of global tags is
        created.
        '''
        # retrieve the dbTree from the main window (yes, I know it is ugly)
        dbTree = self.parent().dbTree

        # just in case...
        self.choseTag.clear()
        if self.checkBoxGlobal.isChecked():
            tagFolder = dbTree.findItem('/TAGS', dbTree.pathColumn)
            if tagFolder:
                tagFolder.loadTagList()
                for tag in tagFolder.tagList[1:]:
                    self.choseTag.insertItem(tag.split('/TAGS-')[-1])
            else:
                raise 'DELETETAGDIALOG ERROR: the /TAGS folder could not be found'
        else:
            folderName = str(self.editFolder.text())
            if folderName <> '':
                folder = dbTree.findItem(folderName, dbTree.pathColumn)
                if folder:
                    folder.loadTagList()
                    for tag in folder.tagList[1:]:
                        # keep only local tags !!
                        if folderName not in tag:
                            self.choseTag.insertItem(tag)
                else:
                    raise 'DELETETAGDIALOG ERROR: the %s folder could not be found'%folderName

    def accept(self):
        return qt.QDialog.accept(self)

    def reject(self):
        self.editFolder.setText('')
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

        #--- Layout ---#
        self.layoutDialog = qt.QVBoxLayout(self)
        self.layoutNode   = qt.QHBoxLayout(self.layoutDialog)
        self.layoutCheck  = qt.QHBoxLayout(self.layoutDialog)
        self.layoutExit   = qt.QHBoxLayout(self.layoutDialog)

        #--- Node info ---#
        self.labelNode = qt.QLabel('Node Name:', self, 'labelNode')
        self.editNode  = qt.QLineEdit(self, 'editNode')
        self.labelDescription = qt.QLabel('Description:', self, 'labelDescription')
        self.editDescription = qt.QLineEdit(self, 'editDescription')

        #--- Check boxes ---#
        self.checkFolderset = qt.QCheckBox('Folderset', self, 'checkFolderset')
        self.checkSingleVersion = qt.QCheckBox('Single Version', self, 'checkSingleVersion')
        self.checkCreateParents = qt.QCheckBox('Create parents', self, 'checkCreateParents')
        self.checkCreateParents.setChecked(True)

        #--- Exit buttons ---#
        self.buttonCreate = qt.QPushButton('Create', self, 'buttonCreate')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog Window Layout ---#
        self.layoutNode.addWidget(self.labelNode)
        self.layoutNode.addWidget(self.editNode)
        self.layoutNode.addWidget(self.labelDescription)
        self.layoutNode.addWidget(self.editDescription)

        self.layoutCheck.addWidget(self.checkFolderset)
        self.layoutCheck.addWidget(self.checkSingleVersion)
        self.layoutCheck.addWidget(self.checkCreateParents)

        self.layoutExit.addWidget(self.buttonCreate)
        self.layoutExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonCreate,     qt.SIGNAL("clicked()"), self.accept)
        self.connect(self.buttonCancel,     qt.SIGNAL("clicked()"), self.reject)

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
        
    def accept(self):
        self.nodeName = str(self.editNode.text())
        self.description = str(self.editDescription.text())
        self.is_folderset = self.checkFolderset.isChecked()
        self.is_singleVersion = self.checkSingleVersion.isChecked()
        self.createParents = self.checkCreateParents.isChecked()
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
        # It will contain lists of 4 elements: channelID, Since, Until, Payload
        self.objectList = []

        #--- Layout ---#
        self.layoutDialog = qt.QGridLayout(self, 5, 3, 5, -1, 'layoutDialog')
        self.layoutPath = qt.QHBoxLayout()
        self.layoutTime = qt.QHBoxLayout()
        self.layoutButtonPayload = qt.QVBoxLayout()
        self.layoutButtonCondList = qt.QVBoxLayout()
        self.layoutButtonExit = qt.QVBoxLayout()

        #--- Dialog ---#
        self.dialogAddLHCbCond = createLHCbCondDialog(self, 'dialogAddLHCbCond')

        #--- Condition object definition ---#
        self.timeValidator    = guiextras.valKeyValidator(self, 'timeValidator')
        self.channelValidator = qt.QIntValidator(self, 'channelValidator')
        self.channelValidator.setBottom(0)

        self.labelFolder = qt.QLabel('Folder: ', self, 'labelFolder')
        self.editFolder  = qt.QLineEdit(self, 'editFolder')
        self.editFolder.setReadOnly(True)

        self.labelChannelID = qt.QLabel('ChannelID: ', self, 'labelChannelID')
        self.editChannelID  = qt.QLineEdit('0', self, 'editChannelID')
        self.editChannelID.setValidator(self.channelValidator)
        self.editChannelID.setAlignment(qt.Qt.AlignRight)

        self.labelSince = qt.QLabel('Since: ', self, 'labelSince')
        self.editSince  = qt.QLineEdit(str(self.timeValidator.valKeyMin), self, 'editSince')
        self.editSince.setValidator(self.timeValidator)
        self.editSince.setAlignment(qt.Qt.AlignRight)

        self.labelUntil = qt.QLabel('Until: ', self, 'labelUntil')
        self.editUntil  = qt.QLineEdit(str(self.timeValidator.valKeyMax), self, 'editUntil')
        self.editUntil.setValidator(self.timeValidator)
        self.editUntil.setAlignment(qt.Qt.AlignRight)
        
        self.editPayload = qt.QTextEdit(self, 'editPayload')
        self.editPayload.setTextFormat(qt.Qt.PlainText)
        self.editPayload.setFont(qt.QFont("Courier", 10))
        self.editPayload.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Expanding)
        
        self.buttonLoadXml   = qt.QPushButton('Load from file', self, 'buttonLoadXml')
        self.buttonAppendXml = qt.QPushButton('Append from file', self, 'buttonAppendXml')
        self.buttonAddLHCbCond = qt.QPushButton('Insert LHCb condition', self, 'buttonAddLHCbCond')
        self.buttonSaveXml   = qt.QPushButton('Save to file', self, 'buttonSaveXml')
        self.buttonClearXml  = qt.QPushButton('Clear all', self, 'buttonClearXml')

        self.fileDialogXml = qt.QFileDialog(self, 'fileDialogXml')

        #--- Condition object list ---#
        self.tableCondObjects = qttable.QTable(0, 4, self, 'tableCondObjects')
        self.tableCondObjects.setColumnLabels(qt.QStringList.fromStrList(['Payload', 'Since', 'Until', 'ChannelID']))
        self.tableCondObjects.setReadOnly(True)
        self.movePad = guiextras.movePad(self, 'movePad', ['Move Up', 'Move Down', 'Del', 'Add'])
        
        #--- Exit buttons ---#
        self.buttonWrite  = qt.QPushButton('Write', self, 'buttonWrite')
        self.buttonCancel = qt.QPushButton('Cancel', self, 'buttonCancel')

        #--- Dialog window layout---#
        # path
        self.layoutDialog.addMultiCellLayout(self.layoutPath, 0, 0, 0, 2)
        self.layoutPath.addWidget(self.labelFolder)
        self.layoutPath.addWidget(self.editFolder)
        self.layoutPath.addWidget(self.labelChannelID)
        self.layoutPath.addWidget(self.editChannelID)

        # time
        self.layoutDialog.addMultiCellLayout(self.layoutTime, 1, 1, 0, 2)
        self.layoutTime.addWidget(self.labelSince)
        self.layoutTime.addWidget(self.editSince)
        self.layoutTime.addWidget(self.labelUntil)
        self.layoutTime.addWidget(self.editUntil)
        
        # payload
        self.layoutDialog.addMultiCellWidget(self.editPayload, 2, 2, 0, 1)
        self.layoutDialog.addLayout(self.layoutButtonPayload, 2, 2)
        self.layoutButtonPayload.addWidget(self.buttonLoadXml)
        self.layoutButtonPayload.addWidget(self.buttonAppendXml)
        self.layoutButtonPayload.addWidget(self.buttonAddLHCbCond)
        self.layoutButtonPayload.addSpacing(10)
        self.layoutButtonPayload.addWidget(self.buttonSaveXml)
        self.layoutButtonPayload.addWidget(self.buttonClearXml)
        
        # condition objects list
        self.layoutDialog.addWidget(self.movePad, 4, 0)
        self.layoutDialog.addWidget(self.tableCondObjects, 4, 1)

        # exit
        self.layoutDialog.addLayout(self.layoutButtonExit, 4, 2)
        self.layoutButtonExit.addWidget(self.buttonWrite)
        self.layoutButtonExit.addWidget(self.buttonCancel)

        #--- Signals connection ---#
        self.connect(self.buttonCancel,     qt.SIGNAL("clicked()"), self.cancel)
        self.connect(self.buttonLoadXml,    qt.SIGNAL("clicked()"), self.loadXml)
        self.connect(self.buttonAppendXml,  qt.SIGNAL("clicked()"), self.appendXml)
        self.connect(self.buttonAddLHCbCond, qt.SIGNAL("clicked()"), self.addLHCbCond)
        self.connect(self.buttonSaveXml,    qt.SIGNAL("clicked()"), self.saveXml)
        self.connect(self.buttonClearXml,   qt.SIGNAL("clicked()"), self.clearXml)
        self.connect(self.tableCondObjects, qt.SIGNAL("doubleClicked(int, int, int, const QPoint &)"), self.reloadObject)

        self.connect(self.movePad.buttonRight, qt.SIGNAL("clicked()"), self.addObject)
        self.connect(self.movePad.buttonLeft, qt.SIGNAL("clicked()"), self.removeObject)
        self.connect(self.movePad.buttonUp,   qt.SIGNAL("clicked()"), self.moveObjectUp)
        self.connect(self.movePad.buttonDown, qt.SIGNAL("clicked()"), self.moveObjectDown)

        self.connect(self.dialogAddLHCbCond.buttonOK, qt.SIGNAL("clicked()"), self.insertLHCbCondXml)
        
        
    def _readXmlFile(self):
        '''
        Open a file dialog to get an xml file, open the file and return its contents
        '''
        if self.fileDialogXml.exec_loop():
            xmlFile = open(str(self.fileDialogXml.selectedFile()), 'r')
            xmlText = xmlFile.read()
            xmlFile.close()
            return xmlText
        else:
            return None

    def _fillTable(self):
        '''
        Fill the tabel with the contents of the object list.
        '''
        self.tableCondObjects.setNumRows(0)
        nbLines = 0
        nbCols  = self.tableCondObjects.numCols()
        for obj in self.objectList:
            self.tableCondObjects.insertRows(nbLines,1)
            for i in range(nbCols - 1):
                self.tableCondObjects.setText(nbLines, i, str(obj[i]))
            if len(str(obj[-1])) > 20 or str(obj[-1]).count('\n') > 0:
                self.tableCondObjects.setText(nbLines, nbCols - 1, 'Double Click to Display')
            else:
                self.tableCondObjects.setText(nbLines, nbCols - 1, str(obj[-1]))
            nbLines += 1
            
        for i in range(self.tableCondObjects.numCols()):
            self.tableCondObjects.adjustColumn(i)      

    def reset(self, defaultFolder = '', defaultChannelID = 0):
        '''
        Reset everything to initial values.
        '''
        self.objectList = []
        self.setFolderName(defaultFolder)
        self.setDefaultChannelID(defaultChannelID)
        self.setIoV(self.timeValidator.valKeyMin, self.timeValidator.valKeyMax)
        self.clearXml()
        self._fillTable()
        
    def removeObject(self):
        '''
        Remove an object form the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        self.objectList.pop(objectIndex)
        self._fillTable()

    def moveObjectUp(self):
        '''
        Move the selected object up in the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        colIndex    = self.tableCondObjects.currentColumn()
        if objectIndex > 0:
            tmp = self.objectList[objectIndex - 1]
            self.objectList[objectIndex - 1] = self.objectList[objectIndex]
            self.objectList[objectIndex] = tmp
            self._fillTable()
            self.tableCondObjects.setCurrentCell(objectIndex - 1, colIndex)

    def moveObjectDown(self):
        '''
        Move the selected object down in the list
        '''
        objectIndex = self.tableCondObjects.currentRow()
        colIndex    = self.tableCondObjects.currentColumn()
        if objectIndex < len(self.objectList) - 1:
            tmp = self.objectList[objectIndex + 1]
            self.objectList[objectIndex + 1] = self.objectList[objectIndex]
            self.objectList[objectIndex] = tmp
            self._fillTable()
            self.tableCondObjects.setCurrentCell(objectIndex + 1, colIndex)
    
    def reloadObject(self, row, col, mouseButton, mousePos):
        '''
        Fill the parameters with the informations from the object selected from the table.
        '''
        loadedObject = self.objectList[row]

        self.editChannelID.setText(str(loadedObject[0]))
        self.editSince.setText(str(loadedObject[1]))
        self.editUntil.setText(str(loadedObject[2]))
        self.editPayload.setText(str(loadedObject[3]))

        
    def loadXml(self):
        '''
        Load an xml file and dump its content in the payload editor
        '''
        newText = self._readXmlFile()
        if newText is None:
            pass
        else:
            self.editPayload.setText(newText)

    def appendXml(self):
        '''
        Load an xml file and append its content to the contents of the payload editor
        '''
        newText = self._readXmlFile()
        if newText is None:
            pass
        else:
            self.editPayload.append(newText)

    def addLHCbCond(self):
        '''
        Open a dialog to edit an LHCb condition.
        '''
        self.dialogAddLHCbCond.reset()
        self.dialogAddLHCbCond.show()

    def insertLHCbCondXml(self):
        '''
        Insert the xml string generated by the LHCb condition editor at the current
        place in the payload editor.
        '''
        self.editPayload.insert(self.dialogAddLHCbCond.createXml())
        self.dialogAddLHCbCond.hide()

    def saveXml(self):
        '''
        Proposes to save the current contents of the payload editor into a file.
        '''
        saveFileName =  str(qt.QFileDialog.getSaveFileName('.'))
        if saveFileName == '':
            pass
        else:
            xmlFile = open(saveFileName, 'w')
            xmlFile.write(str(self.editPayload.text()))
            xmlFile.close()

    def clearXml(self):
        '''
        Clear the contents of the payload editor
        '''
        self.editPayload.setText('')

    def addObject(self):
        '''
        Add a new object to the list of condition objects to write
        '''
        channelID = str(self.editChannelID.text())
        since     = str(self.editSince.text())
        until     = str(self.editUntil.text())
        payload   = str(self.editPayload.text())

        if '' not in [channelID, since, until, payload]:
            # the order must be compatible with the one given in conddbui.storeXMLStringList()
            newObject = [payload, long(since), long(until), int(channelID)]
            self.objectList.append(newObject)
            self._fillTable()
        else:
            errorMsg = qt.QMessageBox('conddbui.py',\
                                      'At least one field is empty\nPlease give all the necessary information to create a new object.',\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()

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

    def setXmlContents(self, xmlText):
        '''
        Set the contents of the xml editor
        '''
        self.editPayload.setText(xmlText)

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
#             CONDDBCONNECTDIALOG             #
#=============================================#

class condDBConnectDialog(qt.QDialog):
    '''
    This dialog allows to give the connection parameter of a Condition
    Database and open it. It is also possible to save these parameters
    in a specific session, and load or delete a saved session.
    '''
    def __init__(self, parent, name = 'condDBConnectDialog'):
        '''
        Initialisation of the dialog window
        '''
        qt.QDialog.__init__(self, parent, name)

        # The string used to connect to the condDB        
        self.connectString = ''

        # The flag telling if the information given will be used to create a new DB
        self.create_new_db = False

        #--- Layout ---#
        self.layoutDialog = qt.QGridLayout(self, 9, 9, 5, -1, 'layoutDialog')

        #--- Stored Sessions ---#
        self.fileSessions = '../python/sessions.dbm'
        self.labelSession = qt.QLabel('Session: ', self, 'labelSession')
        self.choseSession = qt.QComboBox(self, 'choseSession')
        self.choseSession.setEditable(True)
        self.choseSession.setAutoCompletion(True)
        self.choseSession.setInsertionPolicy(qt.QComboBox.NoInsertion)

        # Retrieving the saved sessions...
        storage = shelve.open(self.fileSessions)
        self.choseSession.insertStringList(qt.QStringList.fromStrList(storage.keys()))
        storage.close()
        self.choseSession.listBox().sort(True)

        #--- Backend part ---#
        # So far, there is only 3 backends available. Beware that the signification of
        # the connection parameters may change between them. The nomenclature used here
        # is the one used by POOL: server, schema, user, database name.
        # WARNING: for security reasons, the password must be set in the authentication.xml
        # file.
        self.labelBackend = qt.QLabel('BackEnd: ', self, 'labelBackend')
        self.choseBackend = qt.QComboBox(self, 'choseBackend')
        backendNames = qt.QStringList.fromStrList(['MySQL', 'Oracle', 'SQLite'])
        self.choseBackend.insertStringList(backendNames)

        #--- Server ---#
        self.labelServer = qt.QLabel('Server: ', self, 'labelServer')
        self.editServer  = qt.QLineEdit(self, 'editServer')

        #--- Schema ---#
        self.labelSchema = qt.QLabel('Schema: ', self, 'labelSchema')
        self.editSchema  = qt.QLineEdit(self, 'editSchema')
        # SQLite's "schema" is stored on a file. It is thus handy to
        # have a file dialog available to get it.
        self.buttonSchema = qt.QPushButton('...', self, 'buttonSchema')
        self.buttonSchema.setMaximumWidth(30)
        self.fileDialogSchema = qt.QFileDialog(self, 'fileDialogSchema', True)

        #--- User ---#
        self.labelUser = qt.QLabel('User: ', self, 'labelUser')
        self.editUser  = qt.QLineEdit(self, 'editUser')
        
        #--- Database Name ---#
        self.labelDBName = qt.QLabel('Database Name: ', self, 'labelDBName')
        self.editDBName  = qt.QLineEdit(self, 'editDBName')

        #--- Action Buttons ---#
        # save the current session
        self.buttonSave   = qt.QPushButton('Save Session',   self, 'buttonSave')
        # delete the current session
        self.buttonDelete = qt.QPushButton('Delete Session', self, 'buttonDelete')
        # connect to the DB and exit
        self.buttonOpenDB = qt.QPushButton('Open DB', self, 'buttonOpenDB')
        # create a new DB and exit
        self.buttonNewDB = qt.QPushButton('Create New DB', self, 'buttonNewDB')
        # exit whithout doing anything
        self.buttonCancel = qt.QPushButton('Cancel',  self, 'buttonCancel')
        self.layoutButton = qt.QVBoxLayout()


        #--- Dialog Layout ---#
        self.layoutDialog.addWidget(self.labelSession, 0, 0)
        self.layoutDialog.addWidget(self.choseSession, 0, 2)

        frameSeparator = qt.QFrame(self)
        frameSeparator.setFrameShape(qt.QFrame.HLine)
        self.layoutDialog.addMultiCellWidget(frameSeparator, 1, 1, 0, 3)
        
        self.layoutDialog.addWidget(self.labelBackend, 2, 0)
        self.layoutDialog.addWidget(self.choseBackend, 2, 2)

        self.layoutDialog.addWidget(self.labelServer,  3, 0)
        self.layoutDialog.addWidget(self.editServer,   3, 2)

        self.layoutDialog.addWidget(self.labelSchema,  4, 0)
        self.layoutDialog.addWidget(self.editSchema,   4, 2)
        self.layoutDialog.addWidget(self.buttonSchema, 4, 3)

        self.layoutDialog.addWidget(self.labelUser, 5, 0)
        self.layoutDialog.addWidget(self.editUser,  5, 2)

        self.layoutDialog.addWidget(self.labelDBName, 6, 0)
        self.layoutDialog.addWidget(self.editDBName,  6, 2)

        frameSeparator = qt.QFrame(self)
        frameSeparator.setFrameShape(qt.QFrame.HLine)
        self.layoutDialog.addMultiCellWidget(frameSeparator, 7, 7, 0, 3)

        labelPassword = qt.QLabel('Password must be set in your authentication.xml file', \
                                  self, 'labelPassword')
        self.layoutDialog.addMultiCellWidget(labelPassword, 8, 8, 0, 3 )
        
        self.layoutDialog.addMultiCell(qt.QSpacerItem(20, 0), 0, 8, 4, 4)

        self.layoutDialog.addMultiCellLayout(self.layoutButton, 1, 7, 5, 5)
        self.layoutButton.addWidget(self.buttonSave)
        self.layoutButton.addWidget(self.buttonDelete)
        self.layoutButton.addSpacing(20)
        self.layoutButton.addWidget(self.buttonOpenDB)
        self.layoutButton.addWidget(self.buttonNewDB)
        self.layoutButton.addWidget(self.buttonCancel)

        #--- Signals and slots connections ---#
        self.connect(self.choseSession, qt.SIGNAL("activated(const QString &)"), self.sessionChanged)
        self.connect(self.choseBackend, qt.SIGNAL("activated(int)"),       self.backendChanged)
        self.connect(self.buttonSchema, qt.SIGNAL("clicked()"),      self.fileSelect)        
        self.connect(self.buttonSave,   qt.SIGNAL("clicked()"),      self.sessionSave)
        self.connect(self.buttonDelete, qt.SIGNAL("clicked()"),      self.sessionDelete)        
        self.connect(self.buttonOpenDB, qt.SIGNAL("clicked()"),      self.openExistingDB)
        self.connect(self.buttonNewDB,  qt.SIGNAL("clicked()"),      self.createNewDB)
        self.connect(self.buttonCancel, qt.SIGNAL("clicked()"),      self.reject)

        #--- Initialise the session ---#
        # load the first of the saved sessions
        self.choseSession.setCurrentItem(0)
        self.choseSession.emit(qt.SIGNAL("activated"),(self.choseSession.currentText(),))

    def sessionChanged(self, newSessionName):
        '''
        Action taken when a new session is selected from the choseSession QComboBox.
        This will load the connection parameters of the saved session and signal the
        possible change of the backend.
        '''
        # As the session name can be edited by the user, one need to check if it already
        # exists or not.
        if self.choseSession.listBox().findItem(newSessionName, qt.Qt.ExactMatch):
            session = str(newSessionName)
            # load the session parameters from file...
            storage = shelve.open(self.fileSessions)
            (backend, server, schema, user, dbname) = storage[session]
            storage.close()
            # check if the loaded backend is recognized
            item = self.choseBackend.listBox().findItem(backend, qt.Qt.ExactMatch)   
            if item:
                # set the retrieved connection parameters
                backendIndex = self.choseBackend.listBox().index(item)
                self.choseBackend.setCurrentItem(backendIndex)
                self.editServer.setText(server)
                self.editSchema.setText(schema)
                self.editUser.setText(user)
                self.editDBName.setText(dbname)
                self.choseBackend.emit(qt.SIGNAL("activated"),(backendIndex,))
            else:
                errorMsg = qt.QMessageBox('conddbui.py',\
                                          '%s is not recognised as a supported Backend'%backend,\
                                          qt.QMessageBox.Warning,\
                                          qt.QMessageBox.Ok,\
                                          qt.QMessageBox.NoButton,\
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()

    def backendChanged(self, newBackendIndex):
        '''
        Modify the dialog window appearence given the selected backend.
        '''
        if self.choseBackend.text(newBackendIndex) == 'SQLite':
            self.editServer.setText('')
            self.editServer.setEnabled(False)
            self.editUser.setText('')
            self.editUser.setEnabled(False)
            self.buttonSchema.setEnabled(True)
            self.buttonNewDB.setEnabled(True)
        else:
            self.editServer.setEnabled(True)
            self.editUser.setEnabled(True)
            self.buttonSchema.setEnabled(False)
            self.buttonNewDB.setEnabled(False)

    def fileSelect(self):
        '''
        Run the file dialog to retrieve the path to a SQLite schema.
        '''
        self.fileDialogSchema.setMode(qt.QFileDialog.ExistingFile)
        if self.fileDialogSchema.exec_loop():
            self.editSchema.setText(self.fileDialogSchema.selectedFile())
      
    def sessionSave(self):
        '''
        Save the current session. The name will be the one presently given in
        the editable choseSession QComboBox. The connection parameters will
        be the ones currently set in the various editable fields.
        '''
        session = str(self.choseSession.currentText())
        backend = str(self.choseBackend.currentText())
        server  = str(self.editServer.text())
        schema  = str(self.editSchema.text())
        user    = str(self.editUser.text())
        dbname  = str(self.editDBName.text())

        storage = shelve.open(self.fileSessions)
        if not storage.has_key(session):
            self.choseSession.insertItem(session)
        storage[session] = (backend, server, schema, user, dbname)
        storage.close()

    def sessionDelete(self):
        '''
        Remove the current session from the persistency storage as well as
        from the list of available sessions. It also balnks out the current
        connection parameters.
        '''
        session = str(self.choseSession.currentText())
        storage = shelve.open(self.fileSessions)
        del storage[session]
        storage.close()

        item = self.choseSession.listBox().findItem(session, qt.Qt.ExactMatch)
        sessionIndex = self.choseSession.listBox().index(item)
        self.choseSession.removeItem(sessionIndex)

        self.choseSession.setCurrentText('')
        self.editServer.setText('')
        self.editSchema.setText('')
        self.editUser.setText('')
        self.editDBName.setText('')

    def createNewDB(self):
        '''
        Preacceptation function which simply set the variable
        "create_new_db" to inform the software that a new DB
        needs to be created.
        '''
        self.create_new_db = True
        self.fileDialogSchema.setMode(qt.QFileDialog.AnyFile)
        if self.fileDialogSchema.exec_loop():
            self.editSchema.setText(self.fileDialogSchema.selectedFile())
            self.accept()

    def openExistingDB(self):
        '''
        Preacceptation function which simply set the variable
        "create_new_db" to inform the software that user wants
        to open an existing DB.
        '''
        self.create_new_db = False
        self.accept()

    def accept(self):
        '''
        Dialog acceptation function. It creates the connectString attribute
        given the current connection parameters.
        '''
        backend = self.choseBackend.currentText()
        server  = self.editServer.text()
        schema  = self.editSchema.text()
        user    = self.editUser.text()
        dbname  = self.editDBName.text()
        self.connectString = '%s://%s;schema=%s;user=%s;dbname=%s'%(backend.lower(), server, schema, user, dbname)
        return qt.QDialog.accept(self)

    def reject(self):
        '''
        Dialog cancelation function.
        '''
        return qt.QDialog.reject(self)

#================================================#
