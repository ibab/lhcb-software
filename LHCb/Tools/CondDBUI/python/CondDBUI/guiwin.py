from PyQt4 import Qt, Qt3Support
import CondDBUI, guitree, guidialogs, guiextras
import os, shelve

####################################################
#                   Main Window                    #
####################################################
class NoException:
    pass

class myWindow(Qt.QMainWindow):
    '''
    Class of the main window. It contains a tree to display the architecture of the CondDB,
    a location editor to navigate "manually" in the CondDB, and a myDBTable widget to display
    the contents of a channel. The connection to a condition database is done via the menu:
    DataBase > Open. All communication with the database are done via a CondDBUI.CondDB object
    called "bridge". Most of it are loaded on demand and cached.
    '''
    def __init__(self, bridge = None, parent = None, flags = Qt.Qt.Window):
        Qt.QMainWindow.__init__(self, parent, flags)
        self.setWindowTitle(Qt.QApplication.instance().objectName())
        self.bridge = None
        self.connectionString = ''
        
        #---- Configurations ----#
        self.confFile = os.path.join(os.environ['HOME'],'.conddbbrowserrc')
        self.old_sessions = ["sqlite_file:$SQLITEDBPATH/DDDB.db/DDDB [r-]",
                             "sqlite_file:$SQLITEDBPATH/LHCBCOND.db/LHCBCOND [r-]",
                             "sqlite_file:$SQLITEDBPATH/SIMCOND.db/SIMCOND [r-]",
                             ]
        self.max_old_sessions = 15
        self.external_editor = "emacs"
        if os.path.exists(self.confFile):
            locals = {}
            try:
                exec '\n'.join(open(self.confFile).read().splitlines()) in {}, locals
            except Exception,x:
                print "Warning: exception reading configuration file '%s'"%self.confFile
                print x
            for k in locals:
                if hasattr(self,k):
                    setattr(self, k, locals[k])
                else:
                    print "Warning: ignoring unknown option '%s'"%k
        #------------------------#
        
        #---- Status Bar ----#
        self.statusBar = Qt.QStatusBar(self)
        self.statusBarMsg = Qt.QLabel(self.statusBar)
        self.statusBar.addPermanentWidget(self.statusBarMsg)
        self.setStatusBar(self.statusBar)
        #--------------------#

        #---- Dialog windows ----#
        self.dialogCreateNewDB  = guidialogs.createCondDBDialog(self,'dialogCreateNewDB')
        self.dialogSliceDB      = guidialogs.slicingDialog(self,'dialogSliceDB')
        self.dialogConnectDB    = guidialogs.condDBConnectDialog(self,'dialogConnectDB')
        self.dialogAddCondition = guidialogs.addConditionDialog(self, 'dialogAddCondition',
                                                                externalEditorCmd = self.external_editor)
        self.dialogCreateNode   = guidialogs.createNodeDialog(self, 'dialogCreateNode')
        self.dialogCreateTag    = guidialogs.createTagDialog(self, 'dialogCreateTag')
        self.dialogDeleteNode   = guidialogs.deleteNodeDialog(self, 'dialogDeleteFolder')
        self.dialogDeleteTag    = guidialogs.deleteTagDialog(self, 'dialogDeleteTag')
        #------------------------#

        #---- Splitter ----#
        self.splitter = Qt.QSplitter(Qt.Qt.Horizontal, self)
        # self.dbTree   = guitree.dbTree(self.bridge, self.splitter, name='CondDB Tree')
        self.dbTree   = guiextras.CondDBTree(self.bridge, self.splitter, name='dbTree')
        self.dbTable  = guiextras.myDBTable(self.splitter)

        self.setCentralWidget(self.splitter)
        #------------------#

        #---- Widgets initialisation ----#
        self.dbTable.reset()
        #--------------------------------#

        #---- Menu ----#
        self.menuOldSessions = Qt3Support.Q3PopupMenu(self, 'menuOldSessions')
        for session in self.old_sessions:
            self.menuOldSessions.insertItem(session)
        self.connect(self.menuOldSessions, Qt.SIGNAL("triggered(QAction *)"), self.openOldSession)

        self.menuDB = Qt3Support.Q3PopupMenu(self, 'menuDB')
        self.menuDB.setTitle("&DataBase")
        self.menuDB.insertItem("&New",    self.createNewDB)
        self.menuDB.insertItem("&Open",   self.openDB)
        self.menuDB.insertItem("&Recent", self.menuOldSessions)
        self.menuDB.insertItem("&Slice",  self.createDBSlice)
        self.menuDB.insertItem("&Close",  self.delBridge)
        self.menuDB.addSeparator()
        self.menuDB.insertItem("&Quit",   self.close)

        self.menuEdit = Qt3Support.Q3PopupMenu(self, 'menuEdit')
        self.menuEdit.setTitle("&Edit")
        self.menuEdit.insertItem("Add &Condition", self.openAddConditionDialog)
        self.menuEdit.insertItem("New &Tag",       self.createNewTag)

        self.menuSU = Qt3Support.Q3PopupMenu(self, 'menuSU')
        self.menuSU.setTitle("&Advanced")
        self.menuSU.insertItem("New &Node",      self.createNewNode)
        self.menuSU.insertItem("Delete &Node",     self.deleteNode)
        self.menuSU.insertItem("Delete &Tag",      self.deleteTag)
        # menuSU.insertItem("Delete &Database", self.deleteDatabase)

        self.menuHelp = Qt3Support.Q3PopupMenu(self, 'menuHelp')
        self.menuHelp.setTitle("&Help")
        self.menuHelp.insertItem("&About", self.aboutconddbui)

        self.menuBar = Qt.QMenuBar(self)
        self.menuDBAct = self.menuBar.addMenu(self.menuDB)
        self.menuEditAct = self.menuBar.addMenu(self.menuEdit)
        self.menuSUAct = self.menuBar.addMenu(self.menuSU)
        self.menuHelpAct = self.menuBar.addMenu(self.menuHelp)

        # Hide menus that are meaningless when no database is available.
        self.menuEditAct.setEnabled(False)
        self.menuSUAct.setEnabled(False)

        self.setMenuBar(self.menuBar)
        #--------------#

        #---- Signal Connections ----#
        self.connect(self.dbTree.tree, Qt.SIGNAL("selectionChanged(Q3ListViewItem *)"), self.resolveSelection)
        self.connect(self.dialogAddCondition.buttonWrite, Qt.SIGNAL("clicked()"), self.writeCondition)
        #----------------------------#

        if bridge:
            self.setBridge(bridge)
        

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
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
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
            self.reportException('guiwin.resolveSelection')
        else:
            self.unsetCursor()


    def writeCondition(self):
        '''
        Write a condition objects to the Database and close the dialog window.
        '''
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
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
            self.reportException('guiwin.writeCondition')
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
            if self.dialogCreateNewDB.exec_():
                self.connectionString = self.dialogCreateNewDB.connectionString
                bridge = CondDBUI.CondDB(self.connectionString, True)
                self.setBridge(bridge)
                #self.editLocation.setEnabled(True)
                #self.buttonGo.setEnabled(True)
        except Exception, details:
            self.reportException('guiwin.createNewDB')


    def openOldSession(self, action):
        '''
        Loads a previously opened session
        '''
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
        try:
            sessionText = str(action.text())
            if sessionText.find('[rw]') != -1:
                is_read_only = False
                sessionText = sessionText.replace('[rw]', '')
            else:
                is_read_only = True
                sessionText = sessionText.replace('[r-]', '')
            self.connectionString = sessionText.strip()
            bridge = CondDBUI.CondDB(self.connectionString, create_new_db = False, readOnly = is_read_only)
            self.setBridge(bridge)
        except Exception, details:
            self.unsetCursor()
            self.reportException('guiwin.openOldSession')
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
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
        try:
            if self.dialogConnectDB.exec_():
                connectionString = self.dialogConnectDB.connectString
                is_read_only = self.dialogConnectDB.buttonLocked.isChecked()
                bridge = CondDBUI.CondDB(connectionString,
                                         create_new_db = False,
                                         readOnly = is_read_only)
                self.setBridge(bridge)
        except Exception, details:
            self.unsetCursor()
            self.reportException('guiwin.openDB')
        else:
            self.unsetCursor()


    def createDBSlice(self):
        '''
        Will create a new CondDB which will be a slice of the active one.
        '''
        if self.bridge:
            # Compatibility check with COOL 1.3.2:
            if 'append' not in dir(CondDBUI.PyCoolCopy):
                self.dialogSliceDB.buttonAppend.setEnabled(False)

            try:
                if self.dialogSliceDB.choseNode.count() == 0:
                    self.dialogSliceDB.loadNodes()

                if self.dialogSliceDB.exec_():
                    schema = str(self.dialogSliceDB.editSchema.text())
                    dbName = str(self.dialogSliceDB.editDBName.text())
                    # This will work with COOL > 2.0 (bug #23597)
                    #connectString = 'sqlite_file:%s/%s'%(schema, dbName)
                    connectString = 'sqlite://;schema=%s;dbname=%s'%(schema, dbName)
                    objList = self.dialogSliceDB.objectList
                    selectionList = []
                    for o in objList:
                        s = CondDBUI.PyCoolCopy.Selection(o['path'], long(o['since']), long(o['until']), tags = o['tag'])
                        selectionList.append(s)
                    copyTool = CondDBUI.PyCoolCopy.PyCoolCopy(self.bridge.db)
                    # reduce the verbosity of PyCoolCopy
                    CondDBUI.PyCoolCopy.log.setLevel( CondDBUI.PyCoolCopy.logging.WARNING )
                    if self.dialogSliceDB.do_copy:
                        copyTool.copy(connectString, selectionList)
                    else:
                        copyTool.append(connectString, selectionList)
                self.dialogSliceDB.reset()
            except IOError, details:
                self.dialogSliceDB.reset()            
                self.reportException('guiwin.createDBSlice')


    def close(self):
        '''
        Save configuration file
        '''
        f = open(self.confFile,"w")
        for k in ["max_old_sessions", "old_sessions", "external_editor"]:
            f.write("%s = %s\n"%(k,repr(getattr(self,k))))
        f.close()
        return Qt.QMainWindow.close(self)

    #-------------------------#

    #--- Menu Edit ---#
    def createNewNode(self):
        '''
        Create a new folder or folderset in the DB
        '''
        try:
            self.dialogCreateNode.reset()
            if self.dialogCreateNode.exec_():
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
            self.reportException('guiwin.createNewNode')


    def createNewTag(self):
        '''
        Create a new tag
        '''
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
        try:
            self.dialogCreateTag.reset()
            item = self.dbTree.tree.selectedItem()
            if isinstance(item, guitree.guiChannel):
                self.dialogCreateTag.editNode.setText(item.parent().fullName)
            else:
                self.dialogCreateTag.editNode.setText(item.fullName)
            self.dialogCreateTag.fillTable()

            if self.dialogCreateTag.exec_():
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
            self.reportException('guiwin.createNewTag')

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
                self.dialogAddCondition.reset(item.fullName, xmlDict, externalEditorCmd = self.external_editor)
            elif isinstance(item, guitree.guiChannel):
                row = self.dbTable.tableDB.currentRow()
                tagName = str(self.dbTable.choseTagName.currentText())
                keyList = self.bridge.getFolderStorageKeys(item.parent().fullName)
                for k in keyList:
                    xmlDict[k] = item.getCondDBCache(tagName)[row]['payload'][k]
                self.dialogAddCondition.reset(item.parent().fullName, xmlDict, item.ID, externalEditorCmd = self.external_editor)
            else:
                errorMsg = Qt.QMessageBox('CondDBUI',
                                          "No COOL folder selected\nInsertion in the CondDB can only be done in existing COOL folder",
                                          Qt.QMessageBox.Warning,
                                          Qt.QMessageBox.Ok,
                                          Qt.QMessageBox.NoButton,
                                          Qt.QMessageBox.NoButton)
                errorMsg.exec_()
                return
            self.dialogAddCondition.show()
        except Exception, details:
            self.reportException('guiwin.openAddConditionDialog')

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

            if self.dialogDeleteNode.exec_():
                self.bridge.deleteNode(self.dialogDeleteNode.nodeName)
                self.dbTree.tree.removeNode(self.dialogDeleteNode.nodeName)

        except Exception, details:
            self.reportException('guiwin.deleteNode')


    def deleteTag(self):
        '''
        Will allow the super user to remove a tag from the CondDB. This does not
        change the content of the DB.
        '''
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
        try:
            item = self.dbTree.tree.selectedItem()
            if isinstance(item, guitree.guiChannel):
                self.dialogDeleteTag.editNode.setText(item.parent().fullName)
            else:
                self.dialogDeleteTag.editNode.setText(item.fullName)

            self.dialogDeleteTag.reloadTags()
            if self.dialogDeleteTag.exec_():
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
            self.reportException('guiwin.deleteTag')

        else:
            self.unsetCursor()


    def deleteDatabase(self):
        '''
        Will allow the super user to delete the active database.
        '''
        self.setCursor(Qt.QCursor(Qt.Qt.WaitCursor))
        try:
            choice = Qt.QMessageBox.warning(self, 'CondDB Browser',
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
            self.reportException('guiwin.deleteDatabase')
        else:
            self.unsetCursor()

    #-----------------------#

    #--- Menu Help ---#
    def aboutconddbui(self):
        app = Qt.QApplication.instance()
        message = '''
        %s %s

        Browser for the LHCb-COOL condition database.

        This is build on top of the Python API to LHCb-COOL: CondDBUI,
        and the Python API to COOL: PyCool.

        The Graphical Library is PyQt %s, based on Qt %s

        Marco Clemencic, Nicolas Gilardi
        '''%(app.objectName(), app.applicationVersion(), Qt.PYQT_VERSION_STR, Qt.qVersion())
        aboutMsg = Qt.QMessageBox(app.objectName(),
                                  message, Qt.QMessageBox.Information,
                                  Qt.QMessageBox.Ok, Qt.QMessageBox.NoButton, Qt.QMessageBox.NoButton)
        aboutMsg.exec_()
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
        if self.old_sessions.count(sessionText) == 0:
            self.old_sessions.insert(0, sessionText)
            if len(self.old_sessions) > self.max_old_sessions:
                self.old_sessions = self.old_sessions[0:self.max_old_sessions]

        # Refresh the old sessions menu
        self.menuOldSessions.clear()
        for session in self.old_sessions:
            self.menuOldSessions.insertItem(session)

        # Show elements that are useful when the DB is accessible
        if self.bridge.readOnly:
            self.menuEditAct.setEnabled(False)
            self.menuSUAct.setEnabled(False)
        else:
            self.menuEditAct.setEnabled(True)
            self.menuSUAct.setEnabled(True)
        self.dbTree.setEnabled(True)
        if bridge:
            self.statusBarMsg.setText(sessionText)
        else:
            self.statusBarMsg.clear()

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
        self.menuEditAct.setEnabled(False)
        self.menuSUAct.setEnabled(False)
        self.dbTree.setEnabled(False)
        self.statusBarMsg.clear()

    def reportException(self, location):
        '''
        Open an error message when an exception is caught
        '''
        import traceback
        errorMsg = Qt.QMessageBox(Qt.QApplication.instance().objectName(),
                                  "Called from '%s':\n%s" % (location,
                                                             traceback.format_exc()),
                                  Qt.QMessageBox.Critical,
                                  Qt.QMessageBox.Ok,
                                  Qt.QMessageBox.NoButton,
                                  Qt.QMessageBox.NoButton)
        errorMsg.exec_()



