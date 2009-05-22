import os, re
from PyQt4 import Qt, Qt3Support
import CondDBUI, guitree
import tempfile

#=============================================#
#               DBLOOKUP READER               #
#=============================================#
def readDBLookup():
    '''
    Read the dblookup.xml file. Returns a dictionary with aliases
    as keys and read/write properties as value.
    '''
    aliasDict = {}
    if 'CORAL_DBLOOKUP_PATH' not in os.environ:
        # we do not know where to find the dblooup.xml file
        return aliasDict
    dblookup = open(os.environ['CORAL_DBLOOKUP_PATH'] + os.sep + 'dblookup.xml', 'r').read()
    expr = re.compile(r'<logicalservice.+?name\s*=\s*"(?P<alias>\S+)".*?accessMode\s*=\s*"(?P<access>\w+)"', re.S)
    dbList = expr.findall(dblookup)
    aliasDict = {}
    for group in dbList:
        aliasDict[group[0]] = group[1]
    return aliasDict

#=============================================#
#               VALKEYVALIDATOR               #
#=============================================#

class valKeyValidator(Qt.QValidator):
    '''
    A validator class for cool.ValidityKey editors
    '''
    def __init__(self, parent):
        Qt.QValidator.__init__(self, parent)
        #self.valKeyMin = long(CondDBUI.cool.ValidityKeyMin)
        self.valKeyMin = CondDBUI.ValidityKeyWrapper(CondDBUI.cool.ValidityKeyMin)
        #self.valKeyMax = long(CondDBUI.cool.ValidityKeyMax)
        self.valKeyMax = CondDBUI.ValidityKeyWrapper(CondDBUI.cool.ValidityKeyMax)

    def validate(self, inputString, cursorPos):
        '''
        An overridden function checking if the given input is a valid
        date-time. The parameter cursorPos is not used.

        The validation is accepting any string as "intermediate", which
        means that string inserted by the user may not be valid.
        '''
        inputValue = str(inputString)
        if re.match('^[1-2]?[0-9]{0,3}-?(0?[0-9]?|1[0-2]?)?-?[0-3]?[0-9]?[ T]?'+
                    '([01]?[0-9]?|2[0-3]?)?:?[0-5]?[0-9]?:?[0-5]?[0-9]?'+ \
                    '(\.[0-9]{0,9})?$' +
                    '|^[+]?[iI]?[nN]?[fF]?$',
                    #'^[0-9]{0,4}-?[0-9]{0,2}-?[0-9]{0,2}[ T]?'+
                    #'[0-9]{0,2}:?[0-9]{0,2}:?[0-9]{0,2}'+
                    #'(\.[0-9]{0,9})?$|^[+]?[iI]?[nN]?[fF]?$',
                    inputValue):
            if re.match(CondDBUI.ValidityKeyWrapper.format_re,inputValue):
                try:
                    inVal = CondDBUI.ValidityKeyWrapper(inputValue)
                    if self.valKeyMin <= inVal <= self.valKeyMax:
                        return (Qt.QValidator.Acceptable, cursorPos)
                except:
                    pass
                return (Qt.QValidator.Invalid, cursorPos)
            else:
                return (Qt.QValidator.Intermediate, cursorPos)
        return (Qt.QValidator.Invalid, cursorPos)
            
    def fixup(self, inputString):
        '''
        Fix the inputString if fixable.
        '''
        completable_datetime = '(?P<year>(?:19|2[01])?[0-9]{2})-(?P<month>(?:0?[0-9]|1[0-2]))-(?P<day>[0-3]?[0-9])[ T]'+ \
                               '(?P<hour>(?:[01]?[0-9]|2[0-3])):(?P<minute>[0-5]?[0-9]):(?P<s>[0-5]?[0-9])'+ \
                               '(?:\.(?P<ns>[0-9]{0,9}))?'
        inputValue = str(inputString)
        if inputValue.strip() == '':
            inputString.remove(0, len(inputString))
            inputString.prepend(str(CondDBUI.ValidityKeyWrapper()))
        else:
            mtch = re.match(completable_datetime,inputValue)
            if mtch:
                y,m,d,H,M,S,ns = mtch.groups()
                if len(y) < 4:
                    if int(y) < 70: y = '20'+y
                    else: y = '19'+y
                if len(m) == 1 : m = '0' + m
                if len(d) == 1 : d = '0' + d
                if len(H) == 1 : H = '0' + H
                if len(M) == 1 : M = '0' + M
                if len(S) == 1 : S = '0' + S
                inputValue = '%s-%s-%s %s:%s:%s'%(y,m,d,H,M,S)
                if ns:
                    inputValue += '.' + ns
                inputString.remove(0, len(inputString))
                inputString.prepend(str(inputValue))
                try:
                    CondDBUI.ValidityKeyWrapper(inputValue)
                except:
                    inputString.remove(0, len(inputString))
                    inputString.prepend(str(self.valKeyMin))
            else:
                inputString.remove(0, len(inputString))
                inputString.prepend(str(self.valKeyMin))
                
                
#=============================================#
#                   MOVEPAD                   #
#=============================================#

class movePad(Qt3Support.Q3VBox):
    '''
    A movePad is a QVbox containing 4 QPushButton. The top and bottom ones
    have a up and down arrow, and the two in the middle have a "x" and a "+"
    on them. The goal of this pad is to provide a unique set of buttons for
    moveUp, moveDown, add and erase actions (in a QTable, for instance).
    '''
    def __init__(self, parent = 0, name = 'modePad', labels = ['^','v','x','+'], flag = Qt.Qt.Widget):
        '''
        Initialisation of the widget
        '''
        Qt3Support.Q3VBox.__init__(self, parent, name, flag)

        self.setSpacing(5)
        self.labels     = labels
        
        self.boxTop     = Qt3Support.Q3HBox(self, 'boxTop')
        self.buttonLeft  = Qt.QPushButton(self.labels[3], self.boxTop, 'buttonAdd')
        self.buttonRight = Qt.QPushButton(self.labels[2], self.boxTop, 'buttonDel')
        
        self.buttonUp   = Qt.QPushButton(self.labels[0], self, 'buttonUp')
        self.buttonDown = Qt.QPushButton(self.labels[1], self, 'buttonDown')

        self.boxTop.setSpacing(5)
        self.buttonLeft.setSizePolicy(Qt.QSizePolicy.Expanding, Qt.QSizePolicy.Fixed)
        self.buttonRight.setSizePolicy(Qt.QSizePolicy.Expanding, Qt.QSizePolicy.Fixed)
        
        self.setSizePolicy(Qt.QSizePolicy.Fixed, Qt.QSizePolicy.Fixed)

    def sizeHint(self):
        '''
        Reimplementation of the virtual function QWidget.sizeHint(). Returns
        the preferred size for the widget ?
        '''
        return Qt.QSize(80,100)

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
        self._updateLabels()

    def setLabelDown(self, label):
        '''
        Set the text of button Down
        '''
        self.label[1] = label
        self._updateLabels()

    def setLabelLeft(self, label):
        '''
        Set the text of button left
        '''
        self.label[2] = label
        self._updateLabels()

    def setLabelRight(self, label):
        '''
        Set the text of button Right
        '''
        self.label[3] = label
        self._updateLabels()

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

class myDBTable(Qt.QSplitter):
    '''
    Class displaying the contents of a CondDB channel. It allows to chose a
    specific tag and a time slice.
    '''
    def __init__(self, parent):
        Qt.QSplitter.__init__(self, parent)

        self.setOrientation( Qt.Qt.Horizontal)
        self.activeChannel = None
        self.timeModified  = False
        self.tagNameList = []

        #--- text viewer ---#
        self.textDB = Qt.QTextEdit(self)
        self.textDB.setAcceptRichText(False)
        self.textDB.setFont(Qt.QFont("Courier", 10))
        self.textDB.setReadOnly(True)
        self.textDB.setSizePolicy(Qt.QSizePolicy.MinimumExpanding, Qt.QSizePolicy.MinimumExpanding)
        #-------------------#

        #--- layout ---#
        self.layoutBrowser = Qt3Support.Q3VBox(self, 'layoutBrowser')
        #--------------#

        #--- filter elements ---#
        self.groupFilter = Qt3Support.Q3VGroupBox('Filter', self.layoutBrowser, 'groupFilter')
        self.layoutFilter  = Qt3Support.Q3Grid(2, self.groupFilter, 'layoutFilter')
        self.layoutFilter.setSpacing(5)
        self.validatorTime = valKeyValidator(self)

        self.labelTimeFrom = Qt.QLabel('From time', self.layoutFilter, 'labelTimeFrom')
        self.editTimeFrom  = Qt.QLineEdit(self.layoutFilter, 'editTimeFrom')
        self.editTimeFrom.setValidator(self.validatorTime)
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeFrom.setAlignment(Qt.Qt.AlignRight)

        self.labelTimeTo = Qt.QLabel('To time', self.layoutFilter, 'labelTimeTo')
        self.editTimeTo  = Qt.QLineEdit(self.layoutFilter, 'editTimeTo')
        self.editTimeTo.setValidator(self.validatorTime)
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.editTimeTo.setAlignment(Qt.Qt.AlignRight)

        self.labelTagName = Qt.QLabel('Tag Name', self.layoutFilter, 'labelTag')
        self.choseTagName = Qt3Support.Q3ComboBox(self.layoutFilter, 'choseTagName')
        self.choseTagName.setEditable(True)
        #self.choseTagName.lineEdit().setAlignment(qt.Qt.AlignRight)
        self.choseTagName.setAutoCompletion(True)
        self.choseTagName.setInsertionPolicy(Qt3Support.Q3ComboBox.NoInsertion)
        self.choseTagName.setEnabled(False)
        self.choseTagName.setAutoResize(True)
        self.defaultTagIndex = 0

        self.labelTagFilter = Qt.QLabel('Hide _auto_', self.layoutFilter, 'labelTagFilter')
        self.checkTagFilter = Qt.QCheckBox('', self.layoutFilter, 'checkTagFilter')
        self.checkTagFilter.setChecked(True)
        #-----------------------#

        #--- table ---#
        self.groupTable = Qt3Support.Q3VGroupBox('IOV Table', self.layoutBrowser, 'groupTable')
        self.columnLabels = [('since', 'Since'), ('until', 'Until'), ('insertion', 'Insertion Time')]
        self.tableDB = Qt3Support.Q3Table(0, 0, self.groupTable, 'tableDB')
        self.tableDB.setReadOnly(True)
        for col in self.columnLabels:
            i = self.tableDB.numCols()
            self.tableDB.insertColumns(i, 1)
            self.tableDB.horizontalHeader().setLabel(i, col[1])
            self.tableDB.adjustColumn(i)
        #self.tableDB.setSizePolicy(Qt.QSizePolicy.Minimum, Qt.QSizePolicy.Minimum)
        self.layoutBrowser.setStretchFactor(self.groupTable, 1)
        #-------------#

        #--- payload selector ---#
        self.groupPayload = Qt3Support.Q3VGroupBox('Payload', self.layoutBrowser, 'groupPayload')
        self.selectPayload = Qt3Support.Q3ListBox(self.groupPayload, 'selectPayload')
        #self.selectPayload.setSizePolicy(Qt.QSizePolicy.Minimum, Qt.QSizePolicy.Minimum)
        self.layoutBrowser.setStretchFactor(self.groupPayload, 1)
        #------------------------#

        #--- export button ---#
        self.buttonExport = Qt.QPushButton('Export to File', self.layoutBrowser, 'buttonExport')
        #---------------------#

        #--- Signal connections ---#
        self.connect(self.choseTagName, Qt.SIGNAL("activated(const QString &)"), self.tagChanged)
        self.connect(self.checkTagFilter, Qt.SIGNAL("toggled (bool)"), self.applyTagFilter)
        self.connect(self.tableDB, Qt.SIGNAL("currentChanged(int, int)"), self.showData)
        self.connect(self.tableDB, Qt.SIGNAL("selectionChanged()"), self.showData)
        self.connect(self.selectPayload, Qt.SIGNAL("selectionChanged()"), self.showData)
        self.connect(self.selectPayload, Qt.SIGNAL("clicked(Q3ListBoxItem *)"), self.showData)
        self.connect(self.editTimeFrom, Qt.SIGNAL("returnPressed()"), self.timeChanged)
        self.connect(self.editTimeTo, Qt.SIGNAL("returnPressed()"), self.timeChanged)
        self.connect(self.buttonExport, Qt.SIGNAL("clicked()"), self.exportPayload)
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
        lastItem = ''
        for tagListItem in self.tagNameList:
            if not (applyFilter and tagListItem.find('_auto_') != -1):
                if tagListItem == '-' * 20 and (lastItem == '-' * 20 or lastItem == ''):
                    # do not add 2 consecutive separators
                    pass
                else:
                    self.choseTagName.insertItem(tagListItem)
                    lastItem = tagListItem

                if tagListItem == currentTagName:
                    currentItemIndex = self.choseTagName.count() - 1

        self.choseTagName.setCurrentItem(currentItemIndex)
        if currentItemIndex == self.defaultTagIndex:
            self.choseTagName.emit(Qt.SIGNAL("activated"),(self.choseTagName.currentText(),))


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
                if len(self.tagNameList) > 0:
                    # insert a separator between different versions.
                    self.tagNameList.append('-' * 20)

        self.applyTagFilter(self.checkTagFilter.isChecked())
        self.choseTagName.setCurrentItem(self.defaultTagIndex)
        if self.activeChannel:
            self.choseTagName.emit(Qt.SIGNAL("activated"),(self.choseTagName.currentText(),))


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
            fromTime = CondDBUI.ValidityKeyWrapper(str(self.editTimeFrom.text()))
            toTime   = CondDBUI.ValidityKeyWrapper(str(self.editTimeTo.text()))

            if tagName.find('---') != -1:
                self.choseTagName.setCurrentItem(self.defaultTagIndex)
                self.choseTagName.emit(Qt.SIGNAL("activated"),(self.choseTagName.currentText(),))
                return

            if fromTime > toTime:
                toTime = fromTime
                self.editTimeTo.setText(self.editTimeFrom.text())

            if self.timeModified or not self.activeChannel.getCondDBCache(tagName):
                self.activeChannel.setCondDBCache(tagName, long(fromTime), long(toTime))

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
            tagName  = str(self.choseTagName.currentText()).split()[0]
            fromTime = CondDBUI.ValidityKeyWrapper(str(self.editTimeFrom.text()))
            toTime   = CondDBUI.ValidityKeyWrapper(str(self.editTimeTo.text()))

            if fromTime > toTime:
                toTime = fromTime
                self.editTimeTo.setText(self.editTimeFrom.text())

            self.activeChannel.setCondDBCache(tagName, long(fromTime), long(toTime))
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
        self.parent().setCursor(Qt.QCursor(Qt.Qt.WaitCursor))

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
                # @todo: make the conversion to Unicode more robust (the src may not be ISO)
                qs = Qt.QTextCodec.codecForName("ISO8859-1").toUnicode(xmlText)
                self.textDB.setPlainText(qs)
                self.buttonExport.setEnabled(True)


    def exportPayload(self):
        '''
        Save the payload data currently displayed in the text viewer to a file.
        '''
        filename = Qt.QFileDialog.getSaveFileName(self)
        if filename:
            try:
                xmlFile = open(str(filename), 'w')
            except Exception, details:
                raise Exception, details
            else:
                # @todo: make the conversion to Unicode more robust (the src may not be ISO)
                qs = self.textDB.toPlainText()
                s = Qt.QTextCodec.codecForName("ISO8859-1").fromUnicode(qs)
                xmlFile.write(str(s))
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
        self.tagNameList = []
        self.choseTagName.clear()
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.setEnabled(False)


#============================================#
#                 CondDBTREE                 #
#============================================#

class CondDBTree(Qt3Support.Q3VBox):
    '''
    Widget displaying the database tree and a text editor with the
    current path.
    '''

    def __init__(self, bridge, parent, name = 'CondDBTree', flag = Qt.Qt.Widget):
        '''
        Initialise the CondDBTree. The bridge parameter is passed to the
        dbTree instance.
        '''
        Qt3Support.Q3VBox.__init__(self, parent, name, flag)

        #--- Path editor ---#
        self.layoutPath = Qt3Support.Q3HBox(self, 'layoutPath')
        self.labelPath = Qt.QLabel(' Path ', self.layoutPath, 'labelPath')
        self.editPath  = Qt.QLineEdit('', self.layoutPath, 'editPath')
        self.layoutPath.setStretchFactor(self.editPath, 1)
        self.buttonGo = Qt.QPushButton('Go', self.layoutPath, 'buttonGo')
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
        self.connect(self.tree, Qt.SIGNAL("expanded(Q3ListViewItem *)"),         self.createLeaves)
        self.connect(self.tree, Qt.SIGNAL("selectionChanged(Q3ListViewItem *)"), self.updatePath)
        self.connect(self.buttonGo, Qt.SIGNAL("clicked()"),                     self.resolvePath)
        self.connect(self.editPath, Qt.SIGNAL("returnPressed()"),               self.resolvePath)
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
        self.parent().setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
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
                errorMsg = Qt.QMessageBox('CondDBUI',\
                                          '%s\nUnknown path'%str(self.editPath.text()),\
                                          Qt.QMessageBox.Warning,\
                                          Qt.QMessageBox.Ok,\
                                          Qt.QMessageBox.NoButton,\
                                          Qt.QMessageBox.NoButton)
                errorMsg.exec_()
        except Exception, details:
            raise Exception, details

#============================================#
#               ConditionEditor              #
#============================================#

class ConditionEditor(Qt3Support.Q3VBox):

    def __init__(self, parent, name = 'ConditionEditor', f = Qt.Qt.Widget, extension = '', externalEditorCmd = ''):
        Qt3Support.Q3VBox.__init__(self, parent, name, f)

        #--- buttons ---#
        self.layoutButton = Qt3Support.Q3VBox(self, 'layoutButton')
        self.layoutButton1 = Qt3Support.Q3HBox(self.layoutButton, 'layoutButton1')
        self.buttonLoad = Qt.QPushButton('Load', self.layoutButton1)
        self.buttonExport = Qt.QPushButton('Export', self.layoutButton1)
        self.buttonExtEdit = Qt.QPushButton('Edit with emacs', self.layoutButton1)
        self.layoutButton2 = Qt3Support.Q3HBox(self.layoutButton, 'layoutButton2')
        self.buttonCondition = Qt.QPushButton('<condition>', self.layoutButton2)
        self.buttonAlignmentCondition = Qt.QPushButton('<align.condition>', self.layoutButton2)
        self.buttonParam = Qt.QPushButton('<param>', self.layoutButton2)
        self.buttonParamVector = Qt.QPushButton('<paramvector>', self.layoutButton2)
        #---------------#

        #--- editor ---#
        self.xmlEditor = Qt.QTextEdit(self)
        self.xmlEditor.setAcceptRichText(False)
        #--------------#

        #--- Signal connections ---#
        self.connect(self.buttonLoad, Qt.SIGNAL("clicked()"), self.loadFromFile)
        self.connect(self.buttonExport, Qt.SIGNAL("clicked()"), self.exportToFile)
        self.connect(self.buttonExtEdit, Qt.SIGNAL("clicked()"), self.editExternally)
        self.connect(self.buttonCondition, Qt.SIGNAL("clicked()"), self.addCondition)
        self.connect(self.buttonAlignmentCondition, Qt.SIGNAL("clicked()"), self.addAlignmentCondition)
        self.connect(self.buttonParam, Qt.SIGNAL("clicked()"), self.addParam)
        self.connect(self.buttonParamVector, Qt.SIGNAL("clicked()"), self.addParamVector)
        #--------------------------#

        #--- External editor ---#
        self.externalEditorCmd = externalEditorCmd
        self.fileExtension = extension
        #-----------------------#

    def loadFromFile(self):
        '''
        Load the text editor contents from a file
        '''
        filename = Qt.QFileDialog.getOpenFileName(self)
        if filename:
            xmlText = open(str(filename)).read()
            self.setEditorText(xmlText)

    def setEditorText(self, text):
        '''
        Set the contents of the xml editor
        '''
        # @todo: make the conversion to Unicode more robust (the src may not be ISO)
        qs = Qt.QTextCodec.codecForName("ISO8859-1").toUnicode(text)
        self.xmlEditor.document().setPlainText(qs)

    def getEditorText(self):
        '''
        Returns the contents of the xml editor
        '''
        # @todo: make the conversion to Unicode more robust (the src may not be ISO)
        qs = self.xmlEditor.document().toPlainText()
        s = Qt.QTextCodec.codecForName("ISO8859-1").fromUnicode(qs)
        return str(s)

    def exportToFile(self):
        '''
        Saves the contents of the editor to a file
        '''
        filename = Qt.QFileDialog.getSaveFileName(self)
        if filename:
            try:
                xmlFile = open(str(filename), 'w')
            except Exception, details:
                raise Exception, details
            else:
                xmlFile.write(self.getEditorText())
                xmlFile.close()

    def editExternally(self):
        '''
        Export the condition to a temporary file, end edit with emacs.
        '''
        fd,name = tempfile.mkstemp(suffix=self.fileExtension)
        os.fdopen(fd,"w").write(self.getEditorText())
        try:
            if os.system("%s %s"%(self.externalEditorCmd,name)) == 0 :
                xmlText = open(name).read()
                self.setEditorText(xmlText)
        finally:
            os.remove(name)

    def addCondition(self, class_id=5, name="##_CONDITION_NAME_HERE_##"):
        '''
        Add a condition tag to the xml editor, at cursor position
        '''
        conditionTag = '<condition classID="%d" name="%s">\n\n</condition>'%(class_id,name)
        self.xmlEditor.textCursor().insertText(conditionTag)

    def addAlignmentCondition(self, name="##_CONDITION_NAME_HERE_##"):
        '''
        Add a condition tag to the xml editor, at cursor position
        '''
        conditionTag = '<condition classID="6" name="%s">\n'%(name) + \
                       '<paramVector name="dPosXYZ" type="double">0 0 0</paramVector>\n' + \
                       '<paramVector name="dRotXYZ" type="double">0 0 0</paramVector>\n' + \
                       '<paramVector name="pivotXYZ" type="double">0 0 0</paramVector>\n' + \
                       '</condition>'
        self.xmlEditor.textCursor().insertText(conditionTag)

    def addParam(self,name="##_PARAM_NAME_HERE_##",type_id='##_INT_DOUBLE_STRING_##',value=''):
        '''
        Add a param tag to the xml editor, at cursor position
        '''
        paramTag = '<param name="%s" type="%s">%s</param>'%(name,type_id,value)
        self.xmlEditor.textCursor().insertText(paramTag)

    def addParamVector(self,name="##_PARAMVECTOR_NAME_HERE_##",type_id='##_INT_DOUBLE_STRING_##',value=''):
        '''
        Add a paramVector tag to the xml editor, at cursor position
        '''
        paramVectorTag = '<param name="%s" type="%s">%s</param>'%(name,type_id,value)
        self.xmlEditor.textCursor().insertText(paramVectorTag)

