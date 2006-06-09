import qt, qttable
import conddbui

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

class myDBTable(qt.QVBox):
    '''
    Class displaying the contents of a CondDB channel. The list of condition
    objects is given in a table format, with insertion time, IoV, Payload Type
    and Payload. It allows to chose a specific tag and a time slice.
    '''
    def __init__(self, parent, name = 'myDBTable', flags = 0):
        qt.QVBox.__init__(self, parent, name, flags)
        
        self.activeChannel = None
        self.timeModified  = False

        self.navBox = qt.QHBox(self, 'navBox')
        #--- Contents of the navigation box ---#
        self.validatorTime = valKeyValidator(self)
        
        self.labelTimeSlice = qt.QLabel(' Time Slice:    ', self.navBox, 'labelTimeSlice')

        self.labelTimeFrom = qt.QLabel(' from: ', self.navBox, 'labelTimeFrom')
        self.editTimeFrom  = qt.QLineEdit(self.navBox, 'editTimeFrom')
        self.editTimeFrom.setValidator(self.validatorTime)
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeFrom.setAlignment(qt.Qt.AlignRight)

        self.labelTimeTo = qt.QLabel(' to: ', self.navBox, 'labelTimeTo')
        self.editTimeTo  = qt.QLineEdit(self.navBox, 'editTimeTo')
        self.editTimeTo.setValidator(self.validatorTime)
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.editTimeTo.setAlignment(qt.Qt.AlignRight)

        self.labelTagName = qt.QLabel(' Tag Name: ', self.navBox, 'labelTag')
        self.choseTagName = qt.QComboBox(self.navBox, 'choseTagName')
        self.choseTagName.setEditable(True)
        self.choseTagName.lineEdit().setAlignment(qt.Qt.AlignRight)
        self.choseTagName.setAutoCompletion(True)
        self.choseTagName.setInsertionPolicy(qt.QComboBox.NoInsertion)
        self.choseTagName.setEnabled(False)
        self.choseTagName.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Fixed)
        #--------------------------------------#
        
        self.splitterDB = qt.QSplitter(qt.Qt.Vertical, self, 'splitterDB')
        #--- Contents of the splitter ---#
        self.tableDB = qttable.QTable(0,0,self.splitterDB, 'tableDB')
        self.tableDB.setReadOnly(True)

        self.textDB = qt.QTextEdit(self.splitterDB, 'textDB')
        self.textDB.setTextFormat(qt.Qt.PlainText)
        self.textDB.setFont(qt.QFont("Courier", 10))
        self.textDB.setReadOnly(True)
        self.textDB.hide()
        #--------------------------------#
        
        #--- Signal connections ---#
        self.connect(self.choseTagName, qt.SIGNAL("activated(const QString &)"), self.tagChanged)
        self.connect(self.tableDB, qt.SIGNAL("currentChanged(int, int)"), self.showData)
        self.connect(self.tableDB, qt.SIGNAL("valueChanged(int, int)"),   self.showData)
        self.connect(self.editTimeFrom, qt.SIGNAL("returnPressed()"),     self.timeChanged)
        self.connect(self.editTimeTo, qt.SIGNAL("returnPressed()"),       self.timeChanged)
        #--------------------------#

    def initTable(self, columnLabels):
        """
        Set the labels of the table's columns. The column containing the payload MUST be the
        last one. However, its name is irrelevant.
        """
        for i in range(len(columnLabels)):
            l = columnLabels[i]
            self.tableDB.insertColumns(i,1)
            self.tableDB.horizontalHeader().setLabel(i, l)
            self.tableDB.adjustColumn(i)

    def setEnabled(self, enable):
        '''
        Activate or deactivate the tag and time editors.
        '''
        self.editTimeFrom.setEnabled(enable)
        self.editTimeTo.setEnabled(enable)
        self.choseTagName.setEnabled(enable)

    def setTagList(self, tagList):
        '''
        Fill the tag list with the tags given. Set default
        to "HEAD". If not found, set default to the first tag given.
        '''
        if tagList == [] or tagList == ['HEAD']:
            self.choseTagName.clear()
            self.choseTagName.insertItem('HEAD')
            self.choseTagName.setEnabled(False)
        else:
            try:
                defaultIndex = tagList.index('HEAD')
            except ValueError:
                defaultIndex = 0
            
            self.choseTagName.clear()
            self.choseTagName.insertStringList(qt.QStringList.fromStrList(tagList))
            self.choseTagName.setCurrentItem(defaultIndex)
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
            tagName  = str(tag)
            fromTime = long(str(self.editTimeFrom.text()))
            toTime   = long(str(self.editTimeTo.text()))

            if fromTime > toTime:
                toTime = fromTime
                self.editTimeTo.setText(self.editTimeFrom.text())

            if self.timeModified or not self.activeChannel.condDBCache.has_key(tagName):
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

        self.tableDB.setNumRows(0)
        nbLines = 0
        nbCols  = self.tableDB.numCols()
                
        for data in self.activeChannel.condDBCache[tagName]:
            self.tableDB.insertRows(nbLines,1)
            for i in range(nbCols - 1):
                self.tableDB.setText(nbLines,i, str(data[i]))
            if len(str(data[-1])) > 20 or str(data[-1]).count('\n') > 0:
                self.tableDB.setText(nbLines, nbCols - 1, 'Click to Display')
            else:
                self.tableDB.setText(nbLines, nbCols - 1, str(data[-1]))
            nbLines += 1
            
        for i in range(self.tableDB.numCols()):
            self.tableDB.adjustColumn(i)

        self.parent().unsetCursor()

    def showData(self, row, col):
        '''
        Display the contents of the selected condition object. This is useful
        mostly when this content is too large to fit in a table cell.
        '''
        if self.activeChannel:
            tagName = str(self.choseTagName.currentText())
            if col == self.tableDB.numCols() - 1:
                self.textDB.setText(self.activeChannel.condDBCache[tagName][row][-1])
                self.textDB.show()
            else:
                self.textDB.hide()

    def clearTable(self):
        '''
        Erase the content of the table.
        '''
        self.tableDB.setNumRows(0)
        self.textDB.hide()

    def reset(self):
        '''
        Return to the initial state when no DB was open
        '''
        self.clearTable()
        self.editTimeFrom.setText(str(self.validatorTime.valKeyMin))
        self.editTimeTo.setText(str(self.validatorTime.valKeyMax))
        self.setEnabled(False)


#=============================================#
#                   HVSTREE                   #
#=============================================#

class HVSTree(qt.QVBox):
    '''
    Widget displaying a list of tags, from leaf tags to ancestor tags. An option
    allows to hide the automatically generated ancestor tag names.
    '''

    def __init__(self, parent, name = 'HVSTree', flags = 0):
        '''
        Initialise the elements of the HVS tree.
        '''
        qt.QVBox.__init__(self, parent, name, flag)
        
        #--- Contents of the box ---#
        self.tree       = qt.QListView(self)
        self.checkAuto  = qt.QCheckBox('Hide _auto_ tags', self, 'checkAuto')
        #---------------------------#

        #--- signal connection --#
        self.connect(self.checkAuto, qt.SIGNAL("toggled(bool)"), self.hideAutoTags)

    def hideAutoTags(self, hide = False):
        '''
        If "hide" is True, search all tree items starting with '_auto_' and hide
        them. Otherwise, hides nothing.
        '''
        pass














