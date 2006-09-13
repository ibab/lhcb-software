import os, re
import qt, qttable
import conddbui, guitree

#=============================================#
#               DBLOOKUP READER               #
#=============================================#
def readDBLookup():
    '''
    Read the dblookup.xml file. Returns a dictionary with aliases
    as keys and read/write properties as value.
    '''
    dblookup = open(os.environ['CORAL_DBLOOKUP_PATH'] + os.sep + 'dblookup.xml', 'r').read()
    expr = re.compile(r'<logicalservice.+?name\s*=\s*"(?P<alias>\S+)".*?accessMode\s*=\s*"(?P<access>\w+)"', re.S)
    dbList = expr.findall(dblookup)
    aliasDict = {}
    for group in dbList:
        aliasDict[group[0]] = group[1]
    return aliasDict


#=============================================#
#               MEMORYTABLEITEM               #
#=============================================#
class memoryTableItem(qttable.QTableItem):
    '''
    A simple QTableItem class with an additional memory content
    allowing to store something more than what is displayed in
    the cell.
    '''
    def __init__(self, table, editType = qttable.QTableItem.OnTyping):
        '''
        initialise the table item
        '''
        qttable.QTableItem.__init__(self, table, editType)
        self.textMemory = qt.QString('')

    def setTextMemory(self, text):
        '''
        set the text memory
        '''
        self.textMemory = qt.QString(text)

#=============================================#
#               VALKEYVALIDATOR               #
#=============================================#

class valKeyValidator(qt.QValidator):
    '''
    A validator class for cool.ValidityKey editors
    '''
    def __init__(self, parent, name = 'valKeyValidator'):
        qt.QValidator.__init__(self, parent, name)
        self.valKeyMin = long(conddbui.cool.ValidityKeyMin)
        self.valKeyMax = long(conddbui.cool.ValidityKeyMax)

    def validate(self, inputString, cursorPos):
        '''
        An overridden function checking if the given input is a valid
        cool.ValidityKey. The parameter cursorPos is not used.

        WARNING: for the moment, an empty string and all non numeric
        characteres are forbiden. The consequence is that the QLineEdit
        widget can not be empty.
        This is done for "esthetical reasons" and may change in the
        future if necessary.
        '''
        inputValue = str(inputString).strip()
        try:
            inVal = long(inputValue)
        except ValueError:
            if inputValue == '':
                self.fixup(inputString)
                return (qt.QValidator.Acceptable, cursorPos)
            else:
                return (qt.QValidator.Invalid, cursorPos)
        
        if self.valKeyMin <= inVal <= self.valKeyMax:
            return (qt.QValidator.Acceptable, cursorPos)
        else:
            self.fixup(inputString)
            return (qt.QValidator.Acceptable, cursorPos)
        
    def fixup(self, inputString):
        '''
        Modify the input value such that it is never greater than
        cool::ValidityKeyMax or smaller than cool::ValidityKeyMin.
        '''

        inputValue = str(inputString).strip()
        if inputValue == '':
            inputString.remove(0, len(inputString))
            inputString.prepend(str(self.valKeyMin))
        elif long(inputValue) < self.valKeyMin:
            inputString.remove(0, len(inputString))
            inputString.prepend(str(self.valKeyMin))
        elif long(inputValue) > self.valKeyMax:
            inputString.remove(0, len(inputString))
            inputString.prepend(str(self.valKeyMax))

#=============================================#
#                   MOVEPAD                   #
#=============================================#

class movePad(qt.QVBox):
    '''
    A movePad is a QVbox containing 4 QPushButton. The top and bottom ones
    have a up and down arrow, and the two in the middle have a "x" and a "+"
    on them. The goal of this pad is to provide a unique set of buttons for
    moveUp, moveDown, add and erase actions (in a QTable, for instance).
    '''
    def __init__(self, parent = 0, name = 'modePad', labels = ['^','v','x','+'], flag = 0):
        '''
        Initialisation of the widget
        '''
        qt.QVBox.__init__(self, parent, name, flag)

        self.setSpacing(5)
        self.labels     = labels
        
        self.buttonUp   = qt.QPushButton(self.labels[0], self, 'buttonUp')
        self.boxCentral = qt.QHBox(self, 'boxCentral')
        self.buttonDown = qt.QPushButton(self.labels[1], self, 'buttonDown')

        self.buttonLeft  = qt.QPushButton(self.labels[2], self.boxCentral, 'buttonDel')
        self.buttonRight = qt.QPushButton(self.labels[3], self.boxCentral, 'buttonAdd')

        self.boxCentral.setSpacing(5)
        self.buttonLeft.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Fixed)
        self.buttonRight.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Fixed)
        
        self.setSizePolicy(qt.QSizePolicy.Fixed, qt.QSizePolicy.Fixed)

    def sizeHint(self):
        '''
        Reimplementation of the virtual function QWidget.sizeHint(). Returns
        the preferred size for the widget ?
        '''
        return qt.QSize(80,100)

    def setLabels(self, labels):
        '''
        Set the labels of the 4 buttons. "labels" is a list of 4 strings
        which will be displayed respectively in up, down, left and right
        buttons.
        '''
        self.labels = labels
        self._updateLabels()

    def setLabelUp(self, label):
        '''
        Set the text of button Up
        '''
        self.label[0] = label
        self.updateLabels()

    def setLabelDown(self, label):
        '''
        Set the text of button Down
        '''
        self.label[1] = label
        self.updateLabels()

    def setLabelLeft(self, label):
        '''
        Set the text of button left
        '''
        self.label[2] = label
        self.updateLabels()

    def setLabelRight(self, label):
        '''
        Set the text of button Right
        '''
        self.label[3] = label
        self.updateLabels()

    def _updateLabels(self):
        '''
        Update the buttons labels.
        '''
        self.buttonUp.setText(self.labels[0])
        self.buttonDown.setText(self.labels[1])
        self.buttonLeft.setText(self.labels[2])
        self.buttonRight.setText(self.labels[3])




#=============================================#
#                  MYDBTABLE                  #
#=============================================#

class myDBTable(qt.QSplitter):
    '''
    Class displaying the contents of a CondDB channel. It allows to chose a
    specific tag and a time slice.
    '''
    def __init__(self, parent, name = 'myDBTable'):
        qt.QSplitter.__init__(self, parent, name)

        self.setOrientation( qt.Qt.Horizontal)
        self.activeChannel = None
        self.timeModified  = False
        self.tagNameList = []

        #--- text viewer ---#
        self.textDB = qt.QTextEdit(self, 'textDB')
        self.textDB.setTextFormat(qt.Qt.PlainText)
        self.textDB.setFont(qt.QFont("Courier", 10))
        self.textDB.setReadOnly(True)
        self.textDB.setSizePolicy(qt.QSizePolicy.MinimumExpanding, qt.QSizePolicy.MinimumExpanding)
        #-------------------#

        #--- layout ---#
        self.layoutBrowser = qt.QVBox(self, 'layoutBrowser')
        #--------------#

        #--- filter elements ---#
        self.groupFilter = qt.QVGroupBox('Filter', self.layoutBrowser, 'groupFilter')
        self.layoutFilter  = qt.QGrid(2, self.groupFilter, 'layoutFilter')
        self.layoutFilter.setSpacing(5)
        self.validatorTime = valKeyValidator(self)

        self.labelTimeFrom = qt.QLabel('From time', self.layoutFilter, 'labelTimeFrom')
        self.editTimeFrom  = qt.QLineEdit(self.layoutFilter, 'editTimeFrom')
        self.editTimeFrom.setValidator(self.validatorTime)
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeFrom.setAlignment(qt.Qt.AlignRight)

        self.labelTimeTo = qt.QLabel('To time', self.layoutFilter, 'labelTimeTo')
        self.editTimeTo  = qt.QLineEdit(self.layoutFilter, 'editTimeTo')
        self.editTimeTo.setValidator(self.validatorTime)
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.editTimeTo.setAlignment(qt.Qt.AlignRight)

        self.labelTagName = qt.QLabel('Tag Name', self.layoutFilter, 'labelTag')
        self.choseTagName = qt.QComboBox(self.layoutFilter, 'choseTagName')
        self.choseTagName.setEditable(True)
        self.choseTagName.lineEdit().setAlignment(qt.Qt.AlignRight)
        self.choseTagName.setAutoCompletion(True)
        self.choseTagName.setInsertionPolicy(qt.QComboBox.NoInsertion)
        self.choseTagName.setEnabled(False)
        self.defaultTagIndex = 0

        self.checkTagFilter = qt.QCheckBox('Hide _auto_', self.layoutFilter, 'labelTagFilter')
        #-----------------------#

        #--- table ---#
        self.groupTable = qt.QVGroupBox('IOV Table', self.layoutBrowser, 'groupTable')
        self.columnLabels = [('since', 'Since'), ('until', 'Until'), ('insertion', 'Insertion Time')]
        self.tableDB = qttable.QTable(0, 0, self.groupTable, 'tableDB')
        self.tableDB.setReadOnly(True)
        for col in self.columnLabels:
            i = self.tableDB.numCols()
            self.tableDB.insertColumns(i, 1)
            self.tableDB.horizontalHeader().setLabel(i, col[1])
            self.tableDB.adjustColumn(i)
        #self.tableDB.setSizePolicy(qt.QSizePolicy.Minimum, qt.QSizePolicy.Minimum)
        self.layoutBrowser.setStretchFactor(self.groupTable, 1)
        #-------------#

        #--- payload selector ---#
        self.groupPayload = qt.QVGroupBox('Payload', self.layoutBrowser, 'groupPayload')
        self.selectPayload = qt.QListBox(self.groupPayload, 'selectPayload')
        #self.selectPayload.setSizePolicy(qt.QSizePolicy.Minimum, qt.QSizePolicy.Minimum)
        self.layoutBrowser.setStretchFactor(self.groupPayload, 1)
        #------------------------#

        #--- export button ---#
        self.buttonExport = qt.QPushButton('Export to File', self.layoutBrowser, 'buttonExport')
        #---------------------#

        #--- Signal connections ---#
        self.connect(self.choseTagName, qt.SIGNAL("activated(const QString &)"), self.tagChanged)
        self.connect(self.checkTagFilter, qt.SIGNAL("toggled (bool)"), self.applyTagFilter)
        self.connect(self.tableDB, qt.SIGNAL("currentChanged(int, int)"), self.showData)
        self.connect(self.tableDB, qt.SIGNAL("selectionChanged()"), self.showData)
        self.connect(self.selectPayload, qt.SIGNAL("selectionChanged()"), self.showData)
        self.connect(self.selectPayload, qt.SIGNAL("clicked(QListBoxItem *)"), self.showData)
        self.connect(self.editTimeFrom, qt.SIGNAL("returnPressed()"), self.timeChanged)
        self.connect(self.editTimeTo, qt.SIGNAL("returnPressed()"), self.timeChanged)
        self.connect(self.buttonExport, qt.SIGNAL("clicked()"), self.exportPayload)
        #--------------------------#

    def setEnabled(self, enable):
        '''
        Activate or deactivate the tag and time editors.
        '''
        self.editTimeFrom.setEnabled(enable)
        self.editTimeTo.setEnabled(enable)
        self.choseTagName.setEnabled(enable)

    def applyTagFilter(self, applyFilter):
        '''
        Remove the _auto_ tags from the choseTagName combo box.
        '''
        currentTagName = str(self.choseTagName.currentText())
        self.choseTagName.clear()
        currentItemIndex = self.defaultTagIndex
        for tagListItem in self.tagNameList:
            if not (applyFilter and tagListItem.find('_auto_') != -1):
                self.choseTagName.insertItem(tagListItem)
                if tagListItem == currentTagName:
                    currentItemIndex = self.choseTagName.count() - 1
        self.choseTagName.setCurrentItem(currentItemIndex)
        if currentItemIndex == self.defaultTagIndex:
            self.choseTagName.emit(qt.SIGNAL("activated"),(self.choseTagName.currentText(),))

    def setTagList(self, tagList):
        '''
        Fill the tag list with the tags given. Set default
        to "HEAD".
        '''
        self.tagNameList = []
        if len(tagList) <= 1:
            self.tagNameList = ['HEAD']
            self.defaultTagIndex = 0
        else:
            for tag in tagList:
                if len(self.tagNameList) > 0:
                    # insert a separator between different versions.
                    self.tagNameList.append('-' * 20)

                ancestors = tag.getAncestorTags()
                if tag.name == 'HEAD':
                    # put the HEAD tag in the list and set it as default tag.
                    self.tagNameList.append('HEAD')
                    self.defaultTagIndex = len(self.tagNameList) - 1
                else:
                    # fill the tag list with all the aliases of the tag.
                    node = tag.path.split('/')[-1]
                    if node == '':
                        node = '/'
                    self.tagNameList.append( '%s     [%s]'%(tag.name, node) )
                    for a in ancestors:
                        node = a.path.split('/')[-1]
                        if node == '':
                            node = '/'
                        self.tagNameList.append( '%s     [%s]'%(a.name, node) )
        for tagListItem in self.tagNameList:
            if not (self.checkTagFilter.isChecked() and tagListItem.find('_auto_') != -1):
                self.choseTagName.insertItem(tagListItem)

        self.applyTagFilter(self.checkTagFilter.isChecked())
        self.choseTagName.setCurrentItem(self.defaultTagIndex)
        if self.activeChannel:
            self.choseTagName.emit(qt.SIGNAL("activated"),(self.choseTagName.currentText(),))


    def tagChanged(self, tag):
        '''
        Function called when the tag name is changed. It updates the condDBCache
        if and only if the time slice has been modified previously or the
        new tag has not been used before.
        When the condDBCache is up to date, the timeModified flag is set to False
        and the function _fillTable is called.
        '''
        if self.activeChannel:
            tagName  = str(tag).split()[0]
            fromTime = long(str(self.editTimeFrom.text()))
            toTime   = long(str(self.editTimeTo.text()))

            if tagName.find('---') != -1:
                self.choseTagName.setCurrentItem(self.defaultTagIndex)
                self.choseTagName.emit(qt.SIGNAL("activated"),(self.choseTagName.currentText(),))
                return

            if fromTime > toTime:
                toTime = fromTime
                self.editTimeTo.setText(self.editTimeFrom.text())

            if self.timeModified or not self.activeChannel.getCondDBCache(tagName):
                self.activeChannel.setCondDBCache(tagName, fromTime, toTime)

            self.timeModified = False
            self._fillTable(tagName)


    def timeChanged(self):
        '''
        Function called each time a new value is set for the time slice boundaries.
        It updates the condDBCache, sets the timeModified flag to True and , if necessary,
        fills the table.
        '''
        self.timeModified = True
        if self.activeChannel:
            tagName  = str(self.choseTagName.currentText())
            fromTime = long(str(self.editTimeFrom.text()))
            toTime   = long(str(self.editTimeTo.text()))

            if fromTime > toTime:
                toTime = fromTime
                self.editTimeTo.setText(self.editTimeFrom.text())

            self.activeChannel.setCondDBCache(tagName, fromTime, toTime)
            self._fillTable(tagName)


    def setActiveChannel(self, channel):
        '''
        Update the activeChannel. If a valid channel is given, the editable fields
        are enabled and the tag list is updated. A signal is then emitted to
        display the contents of the channel for the default tag and the current time
        slice.
        '''
        self.activeChannel = channel
        self.timeChanged() # force channel.condDBCache update


    def _fillTable(self, tagName):
        '''
        Private function that shows the contents of the selected channel. The
        condDBCache is updated by the caller function according to the tag
        name and the time slice.
        '''
        self.parent().setCursor(qt.QCursor(qt.Qt.WaitCursor))

        # Reset selection
        self.tableDB.setCurrentCell(0, 0)
        self.selectPayload.setCurrentItem(0)

        self.clearTable()
        nbLines = 0
        nbCols  = self.tableDB.numCols()

        # Get the keys of the payload
        data = self.activeChannel.getCondDBCache(tagName)[0]
        for key in data['payload'].keys():
            self.selectPayload.insertItem(key)

        # Get the contents of the IOV table
        for data in self.activeChannel.getCondDBCache(tagName):
            self.tableDB.insertRows(nbLines,1)
            for i in range(self.tableDB.numCols()):
                key = self.columnLabels[i][0]
                self.tableDB.setText(nbLines, i, str(data[key]))
            nbLines += 1
        for i in range(self.tableDB.numCols()):
            self.tableDB.adjustColumn(i)

        # Select the first entry
        self.tableDB.setCurrentCell(0, 0)
        self.selectPayload.setCurrentItem(0)

        self.parent().unsetCursor()


    def showData(self):
        '''
        Display the contents of the selected condition object in the text box.
        '''
        if self.activeChannel:
            row = self.tableDB.currentRow()
            payloadKey = str(self.selectPayload.currentText())
            tagName = str(self.choseTagName.currentText()).split()[0]
            # Get the correct payload text from the information given
            if payloadKey != '':
                xmlText = self.activeChannel.getCondDBCache(tagName)[row]['payload'][payloadKey]
                self.textDB.setText(xmlText)
                self.buttonExport.setEnabled(True)


    def exportPayload(self):
        '''
        Save the payload data currently displayed in the text viewer to a file.
        '''
        fileDialogExport = qt.QFileDialog(self, 'fileDialogExport', True)
        fileDialogExport.setMode(qt.QFileDialog.AnyFile)
        if fileDialogExport.exec_loop():
            try:
                xmlFile = open(str(fileDialogExport.selectedFile()), 'w')
            except Exception, details:
                raise Exception, details
            else:
                xmlFile.write(str(self.textDB.text()))
                xmlFile.close()


    def clearTable(self):
        '''
        Erase the content of the table.
        '''
        self.tableDB.setNumRows(0)
        self.selectPayload.clear()
        self.textDB.clear()
        self.buttonExport.setEnabled(False)


    def reset(self):
        '''
        Return to the initial state when no DB was open
        '''
        self.clearTable()
        self.choseTagName.clear()
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.setEnabled(False)


#============================================#
#                 CondDBTREE                 #
#============================================#

class CondDBTree(qt.QVBox):
    '''
    Widget displaying the database tree and a text editor with the
    current path.
    '''

    def __init__(self, bridge, parent, name = 'CondDBTree', flag = 0):
        '''
        Initialise the CondDBTree. The bridge parameter is passed to the
        dbTree instance.
        '''
        qt.QVBox.__init__(self, parent, name, flag)

        #--- Path editor ---#
        self.layoutPath = qt.QHBox(self, 'layoutPath')
        self.labelPath = qt.QLabel(' Path ', self.layoutPath, 'labelPath')
        self.editPath  = qt.QLineEdit('', self.layoutPath, 'editPath')
        self.layoutPath.setStretchFactor(self.editPath, 1)
        self.buttonGo = qt.QPushButton('Go', self.layoutPath, 'buttonGo')
        # There is no clear use for this button, so hide it for now
        self.buttonGo.hide()

        self.setEnabled(False)
        #-------------------#

        #--- Database Tree ---#
        self.tree = guitree.dbTree(bridge, self, 'tree')
        self.setStretchFactor(self.tree, 1)
        self.tree.addColumn('Name')
        self.tree.addColumn('Version Style')
        self.tree.setRootIsDecorated(True)
        #---------------------#

        #--- Signal connections ---#
        self.connect(self.tree, qt.SIGNAL("expanded(QListViewItem *)"),         self.createLeaves)
        self.connect(self.tree, qt.SIGNAL("selectionChanged(QListViewItem *)"), self.updatePath)
        self.connect(self.buttonGo, qt.SIGNAL("clicked()"),                     self.resolvePath)
        self.connect(self.editPath, qt.SIGNAL("returnPressed()"),               self.resolvePath)
        #--------------------------#

    def setEnabled(self, enable = True):
        '''
        Enable or disable the path editor
        '''
        self.editPath.setText('')
        self.editPath.setEnabled(enable)
        self.buttonGo.setEnabled(enable)


    def createLeaves(self, treeElem):
        '''
        Build the subelements of the selected tree element
        '''
        self.parent().setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            if isinstance(treeElem, guitree.guiFolder):
                treeElem.fillFolder()
        except Exception, details:
            self.parent().unsetCursor()
            raise Exception, details
        else:
            self.parent().unsetCursor()


    def updatePath(self, treeElem):
        '''
        Updates the path editor contents.
        '''
        self.editPath.setText(treeElem.fullName)


    def resolvePath(self):
        '''
        Reads the contents of the path editor (if edited by the user) and resolve the
        given path to select the relevent tree element. If the path is unknown, a warning
        message appears.
        '''
        try:
            fullPath = str(self.editPath.text())
            treeElem = self.tree.pathFinder(fullPath)
            if treeElem:
                self.tree.setSelected(treeElem, True)
                self.tree.ensureItemVisible(treeElem)
            else:
                errorMsg = qt.QMessageBox('conddbui.py',\
                                          '%s\nUnknown path'%str(self.editLocation.text()),\
                                          qt.QMessageBox.Warning,\
                                          qt.QMessageBox.Ok,\
                                          qt.QMessageBox.NoButton,\
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
        except Exception, details:
            raise Exception, details

#============================================#
#               ConditionEditor              #
#============================================#

class ConditionEditor(qt.QVBox):

    def __init__(self, parent, name = 'ConditionEditor', f = 0):
        qt.QVBox.__init__(self, parent, name, f)

        #--- buttons ---#
        self.layoutButton = qt.QHBox(self, 'layoutButton')
        self.buttonLoad = qt.QPushButton('Load', self.layoutButton)
        self.buttonExport = qt.QPushButton('Export', self.layoutButton)
        self.buttonCondition = qt.QPushButton('<condition>', self.layoutButton)
        self.buttonParam = qt.QPushButton('<param>', self.layoutButton)
        self.buttonParamVector = qt.QPushButton('<paramvector>', self.layoutButton)
        #---------------#

        #--- editor ---#
        self.xmlEditor = qt.QTextEdit(self, 'xmlEditor')
        #--------------#

        #--- Signal connections ---#
        self.connect(self.buttonLoad, qt.SIGNAL("clicked()"), self.loadFromFile)
        self.connect(self.buttonExport, qt.SIGNAL("clicked()"), self.exportToFile)
        self.connect(self.buttonCondition, qt.SIGNAL("clicked()"), self.addCondition)
        self.connect(self.buttonParam, qt.SIGNAL("clicked()"), self.addParam)
        self.connect(self.buttonParamVector, qt.SIGNAL("clicked()"), self.addParamVector)
        #--------------------------#

    def loadFromFile(self):
        '''
        Load the text editor contents from a file
        '''
        fileDialog = qt.QFileDialog(self, 'fileDialog', True)
        if fileDialog.exec_loop():
            fileName = str(fileDialog.selectedFile())
            xmlText = open(fileName).read()
            self.xmlEditor.clear()
            self.xmlEditor.setText(xmlText)

    def setEditorText(self, text):
        '''
        Set the contents of the xml editor
        '''
        self.xmlEditor.setText(text)

    def getEditorText(self):
        '''
        Returns the contents of the xml editor
        '''
        return self.xmlEditor.text()

    def exportToFile(self):
        '''
        Saves the contents of the editor to a file
        '''
        fileDialogExport = qt.QFileDialog(self, 'fileDialogExport', True)
        fileDialogExport.setMode(qt.QFileDialog.AnyFile)
        if fileDialogExport.exec_loop():
            try:
                xmlFile = open(str(fileDialogExport.selectedFile()), 'w')
            except Exception, details:
                raise Exception, details
            else:
                xmlFile.write(str(self.xmlEditor.text()))
                xmlFile.close()

    def addCondition(self):
        '''
        Add a condition tag to the xml editor, at cursor position
        '''
        conditionTag = '<condition classID="6" name=" ##_CONDITION_NAME_HERE_## ">\n\n</condition>'
        self.xmlEditor.insert(conditionTag)

    def addParam(self):
        '''
        Add a param tag to the xml editor, at cursor position
        '''
        paramTag = '<param name=" ##_PARAM_NAME_HERE_## " type=" ##_INT_DOUBLE_STRING_## "> </param>'
        self.xmlEditor.insert(paramTag)

    def addParamVector(self):
        '''
        Add a paramVector tag to the xml editor, at cursor position
        '''
        paramVectorTag = '<paramVector name=" ##_PARAMVECTOR_NAME_HERE_## " type=" ##_INT_DOUBLE_STRING_## "> </paramVector>'
        self.xmlEditor.insert(paramVectorTag)



















