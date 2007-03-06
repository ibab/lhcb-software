import qt, qttable
import conddbui, guitree, guidialogs, guiextras
import os, shelve

#############################
#     General Variables     #
#############################
versionNumber = '$Name: not supported by cvs2svn $'.split()[1]
if versionNumber == "$":
    versionNumber = 'HEAD'

versionId  = '$Id: guiwin.py,v 1.25 2007-03-06 17:22:56 marcocle Exp $'.split()
if len(versionId) < 4:
    versionDate = 'unknown'
else:
    versionDate = versionId[3]

####################################################
#                   Main Window                    #
####################################################

class myWindow(qt.QMainWindow):
    '''
    Class of the main window. It contains a tree to display the architecture of the CondDB,
    a location editor to navigate "manually" in the CondDB, and a myDBTable widget to display
    the contents of a channel. The connection to a condition database is done via the menu:
    DataBase > Open. All communication with the database are done via a conddbui.CondDB object
    called "bridge". Most of it are loaded on demand and cached.
    '''
    def __init__(self, bridge = None, parent = None, name = 'myWindow', flags = qt.Qt.WType_TopLevel):
        qt.QMainWindow.__init__(self, parent, name, flags)

        self.bridge = bridge
        if bridge:
            self.connectionString = self.bridge.connectionString
        else:
            self.connectionString = ''

        #---- Configurations ----#
        self.externalEditorCmd = "emacs"
        #------------------------#
        
        #---- Status Bar ----#
        self.statusBar = qt.QStatusBar(self, 'statusBar')
        #--------------------#

        #---- Dialog windows ----#
        self.dialogCreateNewDB  = guidialogs.createCondDBDialog(self,'dialogCreateNewDB')
        self.dialogSliceDB      = guidialogs.slicingDialog(self,'dialogSliceDB')
        self.dialogConnectDB    = guidialogs.condDBConnectDialog(self,'dialogConnectDB')
        self.dialogAddCondition = guidialogs.addConditionDialog(self, 'dialogAddCondition',
                                                                externalEditorCmd = self.externalEditorCmd)
        self.dialogCreateNode   = guidialogs.createNodeDialog(self, 'dialogCreateNode')
        self.dialogCreateTag    = guidialogs.createTagDialog(self, 'dialogCreateTag')
        self.dialogDeleteNode   = guidialogs.deleteNodeDialog(self, 'dialogDeleteFolder')
        self.dialogDeleteTag    = guidialogs.deleteTagDialog(self, 'dialogDeleteTag')
        #------------------------#

        #---- Splitter ----#
        self.splitter = qt.QSplitter(qt.Qt.Horizontal, self)
        # self.dbTree   = guitree.dbTree(self.bridge, self.splitter, name='CondDB Tree')
        self.dbTree   = guiextras.CondDBTree(self.bridge, self.splitter, name='dbTree')
        self.dbTable  = guiextras.myDBTable(self.splitter)

        self.setCentralWidget(self.splitter)
        #------------------#

        #---- Widgets initialisation ----#
        self.dbTable.reset()
        #--------------------------------#

        #---- Old Sessions ----#
        #self.sessionFile = os.environ['CONDDBUI_SESSIONS_PATH'] + os.sep + 'sessions.dbm'
        self.sessionFile = os.path.join(os.environ['HOME'],'.conddb_browser_sessions.dbm')
        self.oldSessions = []
        if os.access(self.sessionFile, os.F_OK):
            s = shelve.open(self.sessionFile)
            keys = s.keys()
            keys.sort()
            for k in keys:
                if s[k] != '':
                    self.oldSessions.append(s[k])
            s.close()
            if len(self.oldSessions) > 5:
                self.oldSessions = self.oldSessions[0:5]
        else:
            s = shelve.open(self.sessionFile)
            for i in range(5):
                s[str(i)] = ''
            s.close()
        #----------------------#

        #---- Menu ----#
        self.menuOldSessions = qt.QPopupMenu(self, 'menuOldSessions')
        for session in self.oldSessions:
            self.menuOldSessions.insertItem(session)
        self.connect(self.menuOldSessions, qt.SIGNAL("activated(int)"), self.openOldSession)

        menuDB = qt.QPopupMenu(self, 'menuDB')
        menuDB.insertItem("&New",    self.createNewDB)
        menuDB.insertItem("&Open",   self.openDB)
        menuDB.insertItem("&Recent", self.menuOldSessions)
        menuDB.insertItem("&Slice",  self.createDBSlice)
        menuDB.insertItem("&Close",  self.delBridge)
        menuDB.insertSeparator()
        menuDB.insertItem("&Quit",   self.close)

        menuEdit = qt.QPopupMenu(self, 'menuEdit')
        menuEdit.insertItem("Add &Condition", self.openAddConditionDialog)
        menuEdit.insertItem("New &Tag",       self.createNewTag)

        menuSU = qt.QPopupMenu(self, 'menuSU')
        menuSU.insertItem("New &Node",      self.createNewNode)
        menuSU.insertItem("Delete &Node",     self.deleteNode)
        menuSU.insertItem("Delete &Tag",      self.deleteTag)
        # menuSU.insertItem("Delete &Database", self.deleteDatabase)

        menuHelp = qt.QPopupMenu(self, 'menuHelp')
        menuHelp.insertItem("&About", self.aboutconddbui)

        self.menuBar = qt.QMenuBar(self)
        self.menuBar.insertItem("&DataBase", menuDB)
        self.menuBar.insertItem("&Edit",     menuEdit)
        self.menuBar.insertItem("&Advanced", menuSU)
        self.menuBar.insertItem("&Help",     menuHelp)

        # Hide menus that are meaningless when no database is available.
        self.menuBar.setItemEnabled(self.menuBar.idAt(1), False)
        self.menuBar.setItemEnabled(self.menuBar.idAt(2), False)
        #--------------#

        #---- Signal Connections ----#
        self.connect(self.dbTree.tree, qt.SIGNAL("selectionChanged(QListViewItem *)"), self.resolveSelection)
        self.connect(self.dialogAddCondition.buttonWrite, qt.SIGNAL("clicked()"), self.writeCondition)
        #----------------------------#

    ##################
    # Signal actions #
    ##################

    def resolveSelection(self, treeElem):
        '''
        Identifies which tree element is selected and runs the relevent action.
        Basicaly, if the tree element is a channel, the table will be filled with
        the data inside. If it is something else, the table is cleared and nothing
        else happens.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            if isinstance(treeElem, guitree.guiChannel):
                if not treeElem.parent().tag_loaded:
                    self.dbTable.setEnabled(True)
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
        except IOError, details:
        #except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.resolveSelection', str(Exception), str(details))
        else:
            self.unsetCursor()


    def writeCondition(self):
        '''
        Write a condition objects to the Database and close the dialog window.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            self.bridge.storeXMLStringList(self.dialogAddCondition.folderName, self.dialogAddCondition.objectList)
            # Now we need to update the db tree and the db table.
            treeFolder = self.dbTree.tree.findItem(self.dialogAddCondition.folderName, self.dbTree.tree.pathColumn)
            if treeFolder.channel_loaded:
                activeChannel = self.dbTable.activeChannel
                for obj in self.dialogAddCondition.objectList:
                    channelID = obj['channel']
                    channel = self.dbTree.tree.findItem(self.dialogAddCondition.folderName + '/' + str(channelID),
                                                        self.dbTree.tree.pathColumn)
                    if channel:
                        self.dbTable.setActiveChannel(channel)
                        self.dbTable.timeChanged() # well... not really, but it is simpler like this
                    else:
                        guitree.guiChannel(treeFolder, channelID)
                self.dbTable.setActiveChannel(activeChannel)
            self.dialogAddCondition.hide()
        except IOError, details:
            self.unsetCursor()
            self.catchException('guiwin.writeCondition', str(IOError), str(details))
        else:
            self.unsetCursor()

    ##################
    # Menu functions #
    ##################

    #--- Menu Database ---#
    def createNewDB(self):
        '''
        Open a dialog to create a new SQLite database and open it.
        '''
        try:
            if self.dialogCreateNewDB.exec_loop():
                self.connectionString = self.dialogCreateNewDB.connectionString
                bridge = conddbui.CondDB(self.connectionString, True)
                self.setBridge(bridge)
                #self.editLocation.setEnabled(True)
                #self.buttonGo.setEnabled(True)
        except Exception, details:
            self.catchException('guiwin.createNewDB', str(Exception), str(details))



    def openOldSession(self, itemID):
        '''
        Loads a previously opened session
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            sessionText = str(self.menuOldSessions.text(itemID))
            if sessionText.find('[rw]') != -1:
                is_read_only = False
                sessionText = sessionText.replace('[rw]', '')
            else:
                is_read_only = True
                sessionText = sessionText.replace('[r-]', '')
            self.connectionString = sessionText.strip()
            bridge = conddbui.CondDB(self.connectionString, create_new_db = False, readOnly = is_read_only)
            self.setBridge(bridge)
        except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.openOldSession', str(Exception), str(details))
        else:
            self.unsetCursor()


    def openDB(self):
        '''
        Open a Condition Database for browsing and edition. The function gets a connection
        string from the dialogConnectDB window and creates a new bridge object with
        it. If the link to the CondDB is properly established, then the main window
        can be used. If a connection problem occures, an error message is displayed.
        It is also possible to create a new (empty) condition database using this dialog.
        It is limited to the SQLite backend and cannot override an existing DB.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            if self.dialogConnectDB.exec_loop():
                self.connectionString = self.dialogConnectDB.connectString
                is_read_only = self.dialogConnectDB.buttonLocked.isOn()
                bridge = conddbui.CondDB(self.connectionString, create_new_db = False, readOnly = is_read_only)
                self.setBridge(bridge)
        except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.openDB', str(Exception), str(details))
        else:
            self.unsetCursor()


    def createDBSlice(self):
        '''
        Will create a new CondDB which will be a slice of the active one.
        '''
        if self.bridge:
            # Compatibility check with COOL 1.3.2:
            if 'append' not in dir(conddbui.PyCoolCopy):
                self.dialogSliceDB.buttonAppend.setEnabled(False)

            try:
                if self.dialogSliceDB.choseNode.count() == 0:
                    self.dialogSliceDB.loadNodes()

                if self.dialogSliceDB.exec_loop():
                    schema = str(self.dialogSliceDB.editSchema.text())
                    dbName = str(self.dialogSliceDB.editDBName.text())
                    # This will work with COOL > 2.0 (bug #23597)
                    #connectString = 'sqlite_file:%s/%s'%(schema, dbName)
                    connectString = 'sqlite://;schema=%s;dbname=%s'%(schema, dbName)
                    objList = self.dialogSliceDB.objectList
                    selectionList = []
                    for o in objList:
                        s = conddbui.PyCoolCopy.Selection(o['path'], long(o['since']), long(o['until']), tags = o['tag'])
                        selectionList.append(s)
                    copyTool = conddbui.PyCoolCopy.PyCoolCopy(self.bridge.db)
                    # reduce the verbosity of PyCoolCopy
                    conddbui.PyCoolCopy.log.setLevel( conddbui.PyCoolCopy.logging.WARNING )
                    if self.dialogSliceDB.do_copy:
                        copyTool.copy(connectString, selectionList)
                    else:
                        copyTool.append(connectString, selectionList)
                self.dialogSliceDB.reset()
            except IOError, details:
                self.dialogSliceDB.reset()            
                self.catchException('guiwin.createDBSlice', str(Exception), str(details))


    def close(self, alsoDelete = True):
        '''
        Save the old sessions to the sessions.dbm file and close the window
        '''
        s = shelve.open(self.sessionFile)
        for i in range(len(self.oldSessions)):
            s[str(i)] = self.oldSessions[i]
        s.close()
        return qt.QMainWindow.close(self, alsoDelete)

    #-------------------------#

    #--- Menu Edit ---#
    def createNewNode(self):
        '''
        Create a new folder or folderset in the DB
        '''
        try:
            self.dialogCreateNode.reset()
            if self.dialogCreateNode.exec_loop():
                if self.dialogCreateNode.is_folderset:
                    storageType = 'NODE'
                else:
                    storageType = 'XML'
                if self.dialogCreateNode.is_singleVersion:
                    versioning = 'SINGLE'
                else:
                    versioning = 'MULTI'

                self.bridge.createNode(self.dialogCreateNode.nodeName,
                                       self.dialogCreateNode.description,
                                       storageType, versioning,
                                       self.dialogCreateNode.storageKeys)
                self.dbTree.tree.addNode(self.dialogCreateNode.nodeName, self.dialogCreateNode.createParents)
        except Exception, details:
            self.catchException('guiwin.createNewNode', str(Exception), str(details))


    def createNewTag(self):
        '''
        Create a new tag
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            self.dialogCreateTag.reset()
            item = self.dbTree.tree.selectedItem()
            if isinstance(item, guitree.guiChannel):
                self.dialogCreateTag.editNode.setText(item.parent().fullName)
            else:
                self.dialogCreateTag.editNode.setText(item.fullName)
            self.dialogCreateTag.fillTable()

            if self.dialogCreateTag.exec_loop():
                tagName = str(self.dialogCreateTag.editTag.text())
                childTagList = self.dialogCreateTag.selectedTags[:]
                nodeName = str(self.dialogCreateTag.editNode.text())
                if len(childTagList) > 0:
                    for childTag in childTagList:
                        if childTag[1] == 'HEAD':
                            childTag[1] = self.bridge.generateUniqueTagName(tagName)
                            self.bridge.recursiveTag(childTag[0], childTag[1])
                        self.bridge.createTagRelation(childTag[0], tagName, childTag[1])
                else:
                    self.bridge.recursiveTag(nodeName, tagName)
                # Reload the tags to keep tag lists up to date
                item = self.dbTree.tree.firstChild()
                while item:
                    if isinstance(item, guitree.guiFolder) and item.tag_loaded:
                        # force tags reload
                        item.tag_loaded = False
                        item.loadTagList()
                    item = item.itemBelow()

        except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.createNewTag', str(Exception), str(details))

        else:
            self.unsetCursor()


    def openAddConditionDialog(self):
        '''
        Add a list of condition objects to a folder
        '''
        try:
            item = self.dbTree.tree.selectedItem()

            xmlHeader = '<?xml version="1.0" encoding="ISO-8859-1"?>\n'
            xmlHeader += '<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">\n'
            xmlHeader += '<DDDB>\n</DDDB>\n'
            xmlDict = {}

            if isinstance(item, guitree.guiFolder):
                keyList = self.bridge.getFolderStorageKeys(item.fullName)
                for k in keyList:
                    xmlDict[k] = xmlHeader
                self.dialogAddCondition.reset(item.fullName, xmlDict, externalEditorCmd=self.externalEditorCmd)
            elif isinstance(item, guitree.guiChannel):
                row = self.dbTable.tableDB.currentRow()
                tagName = str(self.dbTable.choseTagName.currentText())
                keyList = self.bridge.getFolderStorageKeys(item.parent().fullName)
                for k in keyList:
                    xmlDict[k] = item.getCondDBCache(tagName)[row]['payload'][k]
                self.dialogAddCondition.reset(item.parent().fullName, xmlDict, item.ID, externalEditorCmd=self.externalEditorCmd)
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
        except Exception, details:
            self.catchException('guiwin.openAddConditionDialog', str(Exception), str(details))

    #----------------#

    #--- Menu Super User ---#
    def deleteNode(self):
        '''
        Will allow the super user to delete a folder or folderset from the CondDB
        '''
        try:
            item = self.dbTree.tree.selectedItem()
            if isinstance(item, guitree.guiChannel):
                self.dialogDeleteNode.reset(item.parent().fullName)
            else:
                self.dialogDeleteNode.reset(item.fullName)

            if self.dialogDeleteNode.exec_loop():
                self.bridge.deleteNode(self.dialogDeleteNode.nodeName)
                self.dbTree.tree.removeNode(self.dialogDeleteNode.nodeName)

        except Exception, details:
            self.catchException('guiwin.deleteNode', str(Exception), str(details))


    def deleteTag(self):
        '''
        Will allow the super user to remove a tag from the CondDB. This does not
        change the content of the DB.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            item = self.dbTree.tree.selectedItem()
            if isinstance(item, guitree.guiChannel):
                self.dialogDeleteTag.editNode.setText(item.parent().fullName)
            else:
                self.dialogDeleteTag.editNode.setText(item.fullName)

            self.dialogDeleteTag.reloadTags()
            if self.dialogDeleteTag.exec_loop():
                tagName  = str(self.dialogDeleteTag.choseTag.currentText())
                nodeName = str(self.dialogDeleteTag.editNode.text())
                self.bridge.deleteTag(nodeName, tagName)

                item = self.dbTree.tree.firstChild()
                while item:
                    if isinstance(item, guitree.guiFolder) and item.tag_loaded:
                        # force tags reload
                        item.tag_loaded = False
                        item.loadTagList()
                    item = item.itemBelow()

        except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.deleteTag', str(Exception), str(details))

        else:
            self.unsetCursor()


    def deleteDatabase(self):
        '''
        Will allow the super user to delete the active database.
        '''
        self.setCursor(qt.QCursor(qt.Qt.WaitCursor))
        try:
            choice = qt.QMessageBox.warning(self, 'CondDB Browser',
                                            "You are about to destroy the current database\nThis action can't be undone !",
                                            "Ok", "Cancel")
            if choice == 0:
                if 'sqlite' in self.connectionString:
                    self.bridge.dropDatabase(self.connectionString)
                    self.delBridge()
                else:
                    raise Exception, "The Browser can only drop SQLite databases"
        except Exception, details:
            self.unsetCursor()
            self.catchException('guiwin.deleteDatabase', str(Exception), str(details))
        else:
            self.unsetCursor()

    #-----------------------#

    #--- Menu Help ---#
    def aboutconddbui(self):
        message = '''
        CondDB Browser %s

        Browser for the LHCb-COOL condition database.

        This is build on top of the Python API to LHCb-COOL: conddbui,
        and the Python API to COOL: PyCool.

        The Graphical Library is PyQt, based on Qt 3.3

        Marco Clemencic, Nicolas Gilardi
        (last change: %s)
        '''%(versionNumber, versionDate)
        aboutMsg = qt.QMessageBox('browser.py', message, qt.QMessageBox.Information,
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
        self.connectionString = self.bridge.connectionString
        self.dbTree.tree.setBridge(bridge)

        # Save the session in the old session list
        sessionText = self.connectionString
        if self.bridge.readOnly:
            sessionText = sessionText + ' [r-]'
        else:
            sessionText = sessionText + ' [rw]'
        if self.oldSessions.count(sessionText) == 0:
            self.oldSessions.insert(0, sessionText)
            if len(self.oldSessions) > 5:
                self.oldSessions = self.oldSessions[0:5]

        # Refresh the old sessions menu
        self.menuOldSessions.clear()
        for session in self.oldSessions:
            self.menuOldSessions.insertItem(session)

        # Show elements that are useful when the DB is accessible
        if self.bridge.readOnly:
            self.menuBar.setItemEnabled(self.menuBar.idAt(1), False)
            self.menuBar.setItemEnabled(self.menuBar.idAt(2), False)
        else:
            self.menuBar.setItemEnabled(self.menuBar.idAt(1), True)
            self.menuBar.setItemEnabled(self.menuBar.idAt(2), True)
        self.dbTree.setEnabled(True)


    def delBridge(self):
        '''
        Reset the bridge object to None
        '''
        if self.bridge:
            self.bridge.closeDatabase()
        self.bridge = None
        self.connectionString = ''
        self.dbTree.tree.setBridge(None)
        self.dbTable.reset()

        # Hide elements that are meaningless when no database is available.
        self.menuBar.setItemEnabled(self.menuBar.idAt(1), False)
        self.menuBar.setItemEnabled(self.menuBar.idAt(2), False)
        self.dbTree.setEnabled(False)

    def catchException(self, location, exception, details):
        '''
        Open an error message when an exception is caught
        '''
        errorMsg = qt.QMessageBox('browser.py',
                                  "%s\n%s:\n%s"%(location, exception, details),
                                  qt.QMessageBox.Critical,
                                  qt.QMessageBox.Ok,
                                  qt.QMessageBox.NoButton,
                                  qt.QMessageBox.NoButton)
        errorMsg.exec_loop()




