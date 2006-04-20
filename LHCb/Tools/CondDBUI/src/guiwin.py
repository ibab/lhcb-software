import qt, qttable
import guitree, guibridge, guidialogs, guiextras
import os.path

#############################
#     General Variables     #
#############################
versionNumber = 'v0r3'
versionDate   = '2006.04.20'
enableSuperUser = False

####################################################
#                   Main Window                    #
####################################################

class myWindow(qt.QMainWindow):
    '''
    Class of the main window. It contains a tree to display the architecture of the CondDB,
    a location editor to navigate "manualy" in the CondDB, and a myDBTable widget to display
    the contents of a channel. The connection to a condition database is done via the menu:
    DataBase > Open. All communication with the database are done via a CoolBridge object.
    Most of it are loaded on demand and cached.
    '''
    def __init__(self, bridge = None, parent = None, name = 'myWindow', flags = qt.Qt.WType_TopLevel):
        qt.QMainWindow.__init__(self, parent, name, flags)

        self.bridge = bridge
        
        #---- Toolbar ----#
        self.toolBar = qt.QToolBar(self)

        self.buttonDB = qt.QPushButton('Disconnect', self.toolBar, 'buttonDB')
        self.labelLocation = qt.QLabel('  Location: ', self.toolBar, 'labelLocation')
        self.editLocation  = qt.QLineEdit('', self.toolBar, 'editLocation')
        self.editLocation.setSizePolicy(qt.QSizePolicy.Expanding, qt.QSizePolicy.Fixed)
        self.buttonGo = qt.QPushButton('Go', self.toolBar, 'buttonGo')

        self.buttonDB.setEnabled(False)
        self.editLocation.setEnabled(False)
        self.buttonGo.setEnabled(False)
        
        self.toolBar.setStretchableWidget(self.editLocation)
        #------------------#

        #---- Menu ----#
        menuDB = qt.QPopupMenu(self, 'menuDB')
        menuDB.insertItem("&Open", self.openDB)
        menuDB.insertItem("&Slice", self.createDBSlice)
        menuDB.insertItem("&Close", self.buttonDBAction)
        menuDB.insertSeparator()
        menuDB.insertItem("&Quit", self, qt.SLOT("close()"))

        # Disable slicing as it is not yet implemented
        menuDB.setItemEnabled(menuDB.idAt(1), False)

        menuEdit = qt.QPopupMenu(self, 'menuEdit')
        menuEdit.insertItem("&New Folder", self.createNewFolder)
        menuEdit.insertItem("&Add Condition", self.openAddConditionDialog)
        menuEdit.insertItem("&Tag", self.createNewTag)

        menuSU = qt.QPopupMenu(self, 'menuSU')
        menuSU.insertItem("Delete &Folder", self.deleteFolder)
        menuSU.insertItem("Delete &Tag", self.deleteTag)
        menuSU.insertItem("Delete &Database", self.deleteDatabase)
        for i in range(menuSU.count()):
            menuSU.setItemEnabled(menuSU.idAt(i), enableSuperUser)

        menuHelp = qt.QPopupMenu(self, 'menuHelp')
        menuHelp.insertItem("&About", self.aboutconddbui)

        self.menuBar = qt.QMenuBar(self)
        self.menuBar.insertItem("&DataBase", menuDB)
        self.menuBar.insertItem("&Edit", menuEdit)
        self.menuBar.insertItem("&SuperUser", menuSU)
        self.menuBar.insertItem("&Help", menuHelp)
        #--------------#

        #---- Dialog windows ----#
        self.dialogConnectDB = guidialogs.condDBConnectDialog(self,'dialogConnectDB')
        self.dialogAddCondition = guidialogs.addConditionDialog(self, 'dialogAddCondition')
        self.dialogCreateFolder = guidialogs.createFolderDialog(self, 'dialogCreateFolder')
        self.dialogCreateTag = guidialogs.createTagDialog(self, 'dialogCreateTag')
        self.dialogDeleteFolder = guidialogs.deleteFolderDialog(self, 'dialogDeleteFolder')
        self.dialogDeleteTag = guidialogs.deleteTagDialog(self, 'dialogDeleteTag')
        #------------------------#

        #---- Splitter ----#
        self.split1  = qt.QSplitter(qt.Qt.Horizontal, self)
        self.dbTree  = guitree.dbTree(self.bridge, self.split1, name='CondDB Tree')
        self.dbTable = guiextras.myDBTable(self.split1)

        self.setCentralWidget(self.split1)
        #------------------#

        #---- Widgets initialisation ----#
        self.dbTree.addColumn('Name')
        self.dbTree.addColumn('Version Style')
        self.dbTree.setRootIsDecorated(True)
        self.dbTable.initTable(['Insertion Time', 'Valid Since', 'Valid Until', 'Payload Type', 'Payload'])
        self.dbTable.reset()
        #--------------------------------#

        #---- Signal Connections ----#
        self.connect(self.dbTree, qt.SIGNAL("expanded(QListViewItem *)"),         self.createLeaves)
        self.connect(self.dbTree, qt.SIGNAL("selectionChanged(QListViewItem *)"), self.resolveSelection)
        self.connect(self.buttonDB, qt.SIGNAL("clicked()"),                       self.buttonDBAction)
        self.connect(self.buttonGo, qt.SIGNAL("clicked()"),                       self.resolvePath)
        self.connect(self.editLocation, qt.SIGNAL("returnPressed()"),             self.resolvePath)
        self.connect(self.dialogAddCondition.buttonWrite, qt.SIGNAL("clicked()"), self.writeCondition)
        #----------------------------#

    ##################
    # Signal actions #
    ##################

    def createLeaves(self, treeElem):
        '''
        Build the subelements of the selected tree element
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))

        if isinstance(treeElem, guitree.guiFolder):
            treeElem.fillFolder()

        self.unsetCursor()
        
    def resolveSelection(self, treeElem):
        '''
        Updates the location bar contents, identifies which tree element is selected,
        and runs the relevent action.
        Basicaly, if the tree element is a channel, the table will be filled with
        the data inside. If it is something else, the table is cleared and nothing
        else happens.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))

        self.editLocation.setText(treeElem.fullName)
        if isinstance(treeElem, guitree.guiChannel):
            self.dbTable.setEnabled(True)
            if not treeElem.parent().tag_loaded:
                treeElem.parent().loadTagList()
                self.dbTable.setTagList(treeElem.parent().tagList[:])
            self.dbTable.setActiveChannel(treeElem)
        else:
            self.dbTable.clearTable()
            self.dbTable.setActiveChannel(None)
            if isinstance(treeElem, guitree.guiFolder):
                self.dbTable.setEnabled(True)
                if not treeElem.tag_loaded:
                    treeElem.loadTagList()
                    self.dbTable.setTagList(treeElem.tagList[:])
            elif isinstance(treeElem, guitree.guiFolderSet):
                self.dbTable.setEnabled(False)

        self.unsetCursor()

    def buttonDBAction(self):
        '''
        Run different action depending on the status of the db connection.
        '''
        if self.buttonDB.text() == 'Disconnect':
            self.delBridge()
            self.buttonDB.setText('Reconnect')
        else:
            bridge = guibridge.CoolBridge(self.dialogConnectDB.connectString)
            self.setBridge(bridge)
            self.buttonDB.setText('Disconnect')
        
    def resolvePath(self):
        '''
        Reads the contents of the location bar (if edited by the user) and resolve the
        given path to select the relevent tree element. If the path is unknown, a warning
        message appears.
        '''
        fullPath = str(self.editLocation.text())
        treeElem = self.dbTree.pathFinder(fullPath)
        if treeElem:
            self.dbTree.setSelected(treeElem, True)
            self.dbTree.ensureItemVisible(treeElem)
        else:
            errorMsg = qt.QMessageBox('conddbui.py',\
                                      '%s\nUnknown path'%self.editLocation.text().ascii(),\
                                      qt.QMessageBox.Warning,\
                                      qt.QMessageBox.Ok,\
                                      qt.QMessageBox.NoButton,\
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()

    def writeCondition(self):
        '''
        Write a condition objects to the Database and close the dialog window.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))

        data_written = self.bridge.writeToDB(self.dialogAddCondition.folderName, self.dialogAddCondition.objectList)

        if data_written == True:
            treeFolder = self.dbTree.findItem(self.dialogAddCondition.folderName, self.dbTree.pathColumn)
            if treeFolder.channel_loaded:
                activeChannel = self.dbTable.activeChannel
                for i in range(len(self.dialogAddCondition.objectList)):
                    channelID = self.dialogAddCondition.objectList[i][0]
                    channel = self.dbTree.findItem(self.dialogAddCondition.folderName + '/' + str(channelID),
                                                   self.dbTree.pathColumn)
                    if channel:
                        self.dbTable.setActiveChannel(channel)
                        self.dbTable.timeChanged() # well... not really, but it is simpler like this
                    else:
                        guitree.guiChannel(treeFolder, channelID)
                self.dbTable.setActiveChannel(activeChannel)
            self.unsetCursor()
        else:
            self.unsetCursor()
            errorMsg = qt.QMessageBox('conddbui.py',
                                      "Impossible to write data:\n%s"%data_written,
                                      qt.QMessageBox.Critical,
                                      qt.QMessageBox.Ok,
                                      qt.QMessageBox.NoButton,
                                      qt.QMessageBox.NoButton)
            errorMsg.exec_loop()

        self.dialogAddCondition.hide()



    ##################
    # Menu functions #
    ##################

    #--- Menu Database ---#
    def createNewDB(self):
        '''
        Will ultimately create a new empty CondDB instance. This will likely
        be limited to SQLite.
        '''
        pass

    def openDB(self):
        '''
        Open a Condition Database for browsing and edition. The function gets a connection
        string from the dialogConnectDB window and creates a new bridge object with
        it. If the link to the CondDB is properly established, then the main window
        can be used. If a connection problem occures, an error message is displayed.
        It is also possible to create a new (empty) condition database using this dialog.
        It is limited to the SQLite backend and can't override an existing DB.
        '''
        if self.dialogConnectDB.exec_loop():
            self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
            if self.dialogConnectDB.create_new_db and os.path.exists(str(self.dialogConnectDB.editSchema.text())):
                messageBoxText  = 'This Schema already exist.\n'
                messageBoxText += 'If you REALLY want to override it, please delete the file\n'
                messageBoxText += str(self.dialogConnectDB.editSchema.text())
                errorMsg = qt.QMessageBox('conddbui.py',
                                        messageBoxText,
                                        qt.QMessageBox.Warning,
                                        qt.QMessageBox.Ok,
                                        qt.QMessageBox.NoButton,
                                        qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                self.openDB()
            else:
                bridge = guibridge.CoolBridge(self.dialogConnectDB.connectString,
                                              self.dialogConnectDB.create_new_db)
                if bridge and bridge.db:
                    self.setBridge(bridge)
                    self.buttonDB.setText('Disconnect')
                    self.buttonDB.setEnabled(True)
                    self.editLocation.setEnabled(True)
                    self.buttonGo.setEnabled(True)
                else:
                    errorMsg = qt.QMessageBox('conddbui.py',
                                            'Unable to open the Database',
                                            qt.QMessageBox.Critical,
                                            qt.QMessageBox.Ok,
                                            qt.QMessageBox.NoButton,
                                            qt.QMessageBox.NoButton)
                    errorMsg.exec_loop()
                    self.openDB()
            self.unsetCursor()


    def createDBSlice(self):
        '''
        Will create a new CondDB which will be a slice of the active one.
        '''
        pass
    #-------------------------#

    #--- Menu Edit ---#
    def createNewFolder(self):
        '''
        Create a new folder or folderset in the DB
        '''
        self.dialogCreateFolder.reset()
        if self.bridge is None:
            errorMsg = qt.QMessageBox('conddbui.py',
                                       "No Database loaded\nPlease open a DB before trying to add something in it",
                                       qt.QMessageBox.Warning,
                                       qt.QMessageBox.Ok,
                                       qt.QMessageBox.NoButton,
                                       qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return

        if self.dialogCreateFolder.exec_loop():
            folder_created = self.bridge.createFolder(self.dialogCreateFolder.folderName,
                                                      self.dialogCreateFolder.is_folderset,
                                                      self.dialogCreateFolder.description,
                                                      self.dialogCreateFolder.is_singleVersion,
                                                      self.dialogCreateFolder.createParents)
            if folder_created <> True:
                errorMsg = qt.QMessageBox('conddbui.py',
                                          "Impossible to create the folder:\n%s"%folder_created,
                                          qt.QMessageBox.Critical,
                                          qt.QMessageBox.Ok,
                                          qt.QMessageBox.NoButton,
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return
            else:
                self.dbTree.addNode(self.dialogCreateFolder.folderName, self.dialogCreateFolder.createParents)

    def createNewTag(self):
        '''
        Create a new tag
        '''
        if self.bridge is None:
            errorMsg = qt.QMessageBox('conddbui.py',
                                       "No Database loaded\nPlease open a DB before trying to tag something in it",
                                       qt.QMessageBox.Warning,
                                       qt.QMessageBox.Ok,
                                       qt.QMessageBox.NoButton,
                                       qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return

        item = self.dbTree.selectedItem()
        if isinstance(item, guitree.guiFolder):
            self.dialogCreateTag.editFolder.setText(item.fullName)

        if self.dialogCreateTag.exec_loop():
            tagName = str(self.dialogCreateTag.editTag.text())
            folderName = str(self.dialogCreateTag.editFolder.text())
            isGlobal = self.dialogCreateTag.checkBoxGlobal.isChecked()

            self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
            tag_created = self.bridge.createTag(tagName, folderName, isGlobal)
            if tag_created <> True:
                self.unsetCursor()
                errorMsg = qt.QMessageBox('conddbui.py',
                                          "Impossible to create the tag:\n%s"%tag_created,
                                          qt.QMessageBox.Critical,
                                          qt.QMessageBox.Ok,
                                          qt.QMessageBox.NoButton,
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return
            else:
                if isGlobal:
                    item = self.dbTree.firstChild()
                    while item:
                        if isinstance(item, guitree.guiFolder) and item.tag_loaded:
                            # force the reload of the tag list
                            item.tag_loaded = False
                            item.loadTagList()
                        item = item.itemBelow()
                else:
                    item = self.dbTree.findItem(folderName, self.dbTree.pathColumn)
                    item.tag_loaded = False
                    item.loadTagList()
                self.resolveSelection(self.dbTree.currentItem())
                self.unsetCursor()


    def openAddConditionDialog(self):
        '''
        Add a list of condition objects to a folder
        '''
        self.dialogAddCondition.reset()
        if self.bridge is None:
            errorMsg = qt.QMessageBox('conddbui.py',
                                       "No Database loaded\nPlease open a DB before trying to add something in it",
                                       qt.QMessageBox.Warning,
                                       qt.QMessageBox.Ok,
                                       qt.QMessageBox.NoButton,
                                       qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return
        else:
            item = self.dbTree.selectedItem()
            if isinstance(item, guitree.guiFolder):
                self.dialogAddCondition.setFolderName(item.fullName)
                self.dialogAddCondition.setDefaultChannelID(0)
            elif isinstance(item, guitree.guiChannel):
                if self.dbTable.textDB.isVisible():
                    row = self.dbTable.tableDB.currentRow()
                    self.dialogAddCondition.setXmlContents(str(self.dbTable.textDB.text()))
                    self.dialogAddCondition.setIoV(str(self.dbTable.tableDB.text(row, 1)),str(self.dbTable.tableDB.text(row, 2))) 
                
                self.dialogAddCondition.setDefaultChannelID(item.ID)
                self.dialogAddCondition.setFolderName(item.parent().fullName)
            else:
                errorMsg = qt.QMessageBox('conddbui.py',
                                          "No COOL folder selected\nInsertion in the CondDB can only be done in existing COOL folder",
                                          qt.QMessageBox.Warning,
                                          qt.QMessageBox.Ok,
                                          qt.QMessageBox.NoButton,
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return
            
            self.dialogAddCondition.show()
    #----------------#

    #--- Menu Super User ---#
    def deleteFolder(self):
        '''
        Will allow the super user to delete a folder or folderset from the CondDB
        '''
        ##
        print 'delete folder'
        ##
        self.dialogDeleteFolder.reset()
        if self.bridge is None:
            errorMsg = qt.QMessageBox('conddbui.py',
                                       "No Database loaded\nPlease open a DB before trying to remove something from it",
                                       qt.QMessageBox.Warning,
                                       qt.QMessageBox.Ok,
                                       qt.QMessageBox.NoButton,
                                       qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return

        if self.dialogDeleteFolder.exec_loop():
            folder_deleted = self.bridge.deleteFolder(self.dialogDeleteFolder.folderName)
            if folder_deleted <> True:
                errorMsg = qt.QMessageBox('conddbui.py',
                                          "Impossible to remove the folder:\n%s"%folder_deleted,
                                          qt.QMessageBox.Critical,
                                          qt.QMessageBox.Ok,
                                          qt.QMessageBox.NoButton,
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return
            else:
                self.dbTree.removeNode(self.dialogDeleteFolder.folderName)


    def deleteTag(self):
        '''
        Will allow the super user to remove a tag from the CondDB. This does not
        change the content of the DB.
        '''
        if self.bridge is None:
            errorMsg = qt.QMessageBox('conddbui.py',
                                       "No Database loaded\nPlease open a DB before trying to tag something in it",
                                       qt.QMessageBox.Warning,
                                       qt.QMessageBox.Ok,
                                       qt.QMessageBox.NoButton,
                                       qt.QMessageBox.NoButton)
            errorMsg.exec_loop()
            return

        item = self.dbTree.selectedItem()
        if isinstance(item, guitree.guiFolder):
            self.dialogDeleteTag.editFolder.setText(item.fullName)

        self.dialogDeleteTag.reloadTags()
        if self.dialogDeleteTag.exec_loop():
            tagName = str(self.dialogDeleteTag.choseTag.currentText())
            folderName = str(self.dialogDeleteTag.editFolder.text())

            self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
            tag_deleted = self.bridge.deleteTag(tagName, folderName, self.dialogDeleteTag.checkBoxGlobal.isChecked())
            if tag_deleted <> True:
                self.unsetCursor()
                errorMsg = qt.QMessageBox('conddbui.py',
                                          "Impossible to delete the tag:\n%s"%tag_deleted,
                                          qt.QMessageBox.Critical,
                                          qt.QMessageBox.Ok,
                                          qt.QMessageBox.NoButton,
                                          qt.QMessageBox.NoButton)
                errorMsg.exec_loop()
                return
            else:
                item = self.dbTree.firstChild()
                while item:
                    if isinstance(item, guitree.guiFolder) and item.tag_loaded:
                        # force tags reload
                        item.tag_loaded = False
                        item.loadTagList()
                    item = item.itemBelow()
                self.unsetCursor()


    def deleteDatabase(self):
        '''
        Will allow the super user to delete the active database.
        '''
        ##
        print 'delete DB'
        ##
        pass
    #-----------------------#

    #--- Menu Help ---#
    def aboutconddbui(self):
        message = '''
        conddbui %s

        User Interface for browsing and editing a COOL Condition Database
        (with some LHCb bias ;-) )

        Nicolas Gilardi, %s'''%(versionNumber, versionDate)
        aboutMsg = qt.QMessageBox('conddbui.py', message, qt.QMessageBox.Information,
                                  qt.QMessageBox.Ok, qt.QMessageBox.NoButton, qt.QMessageBox.NoButton)
        aboutMsg.exec_loop()
        return

    ######################
    # Internal functions #
    ######################

    def setBridge(self, bridge):
        '''
        Set the bridge object which allow to communicate with the CondDB.
        '''
        self.dbTable.reset()
        self.bridge = bridge
        self.dbTree.setBridge(bridge)

    def delBridge(self):
        '''
        Reset the bridge object to None
        '''
        self.bridge = None
        self.dbTree.setBridge(None)
        self.dbTable.reset()





