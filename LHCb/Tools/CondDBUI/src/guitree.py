import qt
import os.path
from   xpmimg import *
##
from time import time
from conddbui import ValidityKeyWrapper
##

#-------------------------------------#
#      Fundamental tree elements      #
#-------------------------------------#

class dbTree(qt.QListView):
    '''
    Class of the DB Tree
    '''
    def __init__(self, bridge, parent = None, name = '', pathColumn = 2, flag = 0):
        '''
        Initialise the DB Tree. The "bridge" is an interface between the user interface
        and the database. It allows to access informations in a standard way. The other
        input parameters are the same as for standard QListView constructor.
        '''
        qt.QListView.__init__(self, parent, name, flag)
        self.pathColumn   = pathColumn
        self.shortName    = name
        self.fullName     = self.shortName
        self.child_loaded = False   # flag to prevent loading data twice.
        if bridge:
            self.setBridge(bridge)
        else:
            self.bridge = None

    def setBridge(self, bridge):
        '''
        Cleans the tree, sets the bridge to the CondDB and starts populating the tree.
        '''
        self.clear()
        self.bridge = bridge
        if self.bridge:
            self.growTree()

    def addNode(self, nodeName, createParents = False):
        '''
        Add a new node to the tree. Creates its parents if needed
        '''
        parentPath, shortName = os.path.split(nodeName)
        parentItem = self.findItem(parentPath, self.pathColumn)
        if parentItem:
            if self.bridge.db.existsFolderSet(nodeName):
                guiFolderSet(parentItem, shortName)
            elif self.bridge.db.existsFolder(nodeName):
                guiFolder(parentItem, shortName)
            else:
                raise Exception, 'ERROR: %s was not found in the CondDB'%nodeName
        else:
            if createParents:
                self.addNode(parentPath, createParents)
                self.addNode(nodeName, createParents)
            else:
                raise Exception, 'ERROR: parent item %s does not exist'%parentPath

    def removeNode(self, nodeName):
        '''
        Remove a node and its subnodes from the tree
        '''
        parentPath, shortName = os.path.split(nodeName)
        parentItem = self.findItem(parentPath, self.pathColumn)
        nodeItem   = self.findItem(nodeName, self.pathColumn)
        if parentItem and nodeItem:
            parentItem.takeItem(nodeItem)
            del nodeItem
        else:
            raise Exception, 'ERROR: node %s does not exist'%nodeName



    def growTree(self):
        '''
        Build the tree elements based on the list of nodes of the CondDB
        '''
        nodes = self.bridge.getAllNodes()
        for nodeName in nodes:
            if nodeName == '/':
                guiFolderSet(self, '')
            else:
                self.addNode(nodeName)
            
    def pathFinder(self, itemPath):
        '''
        Method which return the tree element selected via 
        the location bar.
        '''
        item = self.findItem(itemPath, self.pathColumn)
        if item:
            return item
        else:
            return None
        

class guiTreeElement(qt.QListViewItem):
    '''
    Generic class of all displayable elements of a guiTree object.
    '''
    def __init__(self, parent, name, pix = None):
        '''
        Create a guiTreeElement given a parent which can either be a
        guiTreeElement or a guiTree.
        "name" can be a single identifier or a path separated by "/",
        in which case the object name itself is the last chaine of
        character after the last "/".
        "pathColumn" is the column number where the QListView will
        look for the fullpath of the guiTreeElement.
        "pix" can either be a single QPixmap object or a list of two
        QPixmap objects. In the first case, the pixmap is used to represent
        the object in the tree. In the second case, the first pixmap shows
        the closed stat of the object and the second shows the open state
        of the object in the tree.
        '''
        # test of the types of parameters
        assert isinstance(parent, guiTreeElement) or \
               isinstance(parent, dbTree), \
               'The parent object of a guiTreeElement must be either a guiTreeElement or a dbTree'
        assert isinstance(name, str), 'The name of a guiTreeElement must be a string'

        qt.QListViewItem.__init__(self, parent)
        if isinstance(parent, dbTree):
            pathColumn = parent.pathColumn
            self.shortName = name
            self.fullName  = '/' + name
            self.setText(0, self.fullName)
        else:
            pathColumn = parent.listView().pathColumn
            if name == '':
                # give a default identifier.
                self.shortName = 'NONAME'
            else:
                self.shortName = os.path.basename(name)
                
            if parent.fullName == '/':
                # the parent is the root. No need for another '/' to separate.
                self.fullName = parent.fullName + self.shortName
            else:    
                self.fullName = parent.fullName + '/' + self.shortName
            self.setText(0, self.shortName)
            
        # Fill the QListView extra fields:
        self.setText(1, '')            # version
        self.setText(pathColumn, self.fullName) # full path

        # Load the pixmap if one is given
        if pix:
            try:
                if len(pix) > 1:
                    self.closedPix = pix[0]
                    self.openedPix = pix[1]
                else:
                    self.closedPix = pix[0]
                    self.openedPix = pix[0]
            except:
                self.closedPix = pix
                self.openedPix = pix
            self.setPixmap(0,self.closedPix)

    def setOpen(self, open_it):
        '''
        Changes the object pixmap when opened
        '''
        if open_it:
            self.setPixmap(0, self.openedPix)
        else:
            self.setPixmap(0, self.closedPix)
        qt.QListViewItem.setOpen(self, open_it)


#-------------------------------------#
#   Elements to display in the tree   #
#-------------------------------------#

class guiChannel(guiTreeElement):
    '''
    GUI representation of a CondDB Channel which is the container for
    the condition objects. It is caching the contents of the CondDB
    in a dictionary to avoid reading the CondDB too many times. 
    '''
    def __init__(self, parent, channelID):
        '''
        Create a guiChannel  given a parent which can only be a guiFolder.
        The "channelID" is an integer.
        '''
        guiTreeElement.__init__(self, parent, str(channelID), qt.QPixmap(cond_xpm))
        self.ID          = channelID
        self.condDBCache = {}

    def compare(self, channel, numCol, ascending = False):
        '''
        This function allows to sort correctly the channels when they are displayed
        in the tree. This insures that they are displayed in arithmetic order (1, 2, 3...)
        and not in "alphabetical" order (1, 10, 11...).
        '''
        if numCol == 0:
            diff = self.ID - channel.ID
            if ascending:
                return diff
            else:
                return -diff
        else:
            return guiTreeElement.compare(self, channel, numCol, ascending)


    def setCondDBCache(self, tagName, fromTime, toTime):
        '''
        Fills the condDB cache according to a tagName and a time slice. The tagName will be
        used as a dictionary key for further reading of the data. The time slice information
        are only used when retrieving data from the CondDB and are not kept. This means that
        for a given tag, each time a new time slice is set, the content of the dictionary
        entry are overwritten.
        This is not optimal but for the moment it works fine.
        '''
        # special case of the 'HEAD' or empty tag.
        if tagName == '' or tagName == 'HEAD':
            tag = 'HEAD'
        else:
            folder = self.listView().bridge.db.getFolder(self.parent().fullName)
            try:
                tag = folder.resolveTag(tagName)
            except Exception, details:
                raise Exception, details
            
        self.condDBCache[tag] = []
        objList = self.listView().bridge.getPayloadList(self.parent().fullName, fromTime, toTime, self.ID, tag)
        for obj in objList:
            payload = obj[0]
            since   = ValidityKeyWrapper(obj[1])
            until   = ValidityKeyWrapper(obj[2])
            insert  = obj[4]
            # If something changes here, be careful to update the table display
            # as well.
            self.condDBCache[tag].append({'since':     since,
                                          'until':     until,
                                          'payload':   payload,
                                          'insertion': "%d-%02d-%02d %02d:%02d:%02d"%(insert.year(),insert.month(),insert.day(),
                                                                                      insert.hour(),insert.minute(),insert.second())})

    def getCondDBCache(self, tagName):
        """
        Retrieve the contents of the channel's CondDBCache for the given tag name. If the
        tag is an ancestor, the real tag name is automatically resolved.
        """
        # special case of the 'HEAD' or empty tag.
        if tagName == '' or tagName == 'HEAD':
            tag = 'HEAD'
        else:
            folder = self.listView().bridge.db.getFolder(self.parent().fullName)
            try:
                tag = folder.resolveTag(tagName)
            except Exception, details:
                raise Exception, details
        if self.condDBCache.has_key(tag):
            return self.condDBCache[tag]
        else:
            return None
            
class guiFolder(guiTreeElement):
        '''
        A Folder in the CondDB. It contains Channels
        '''
        def __init__(self, parent, name):
            '''
            Creates a guiFolder.
            '''        
            guiTreeElement.__init__(self, parent, name, qt.QPixmap(folder_xpm))
            self.setExpandable(True)
            self.channel_loaded = False
            self.tag_loaded = False
            self.tagList = []
            self.versioning  = -1 # 0 for single version, 1 for multi version, -1 for no information
            self.setText(1, '  ?')

        def loadTagList(self):
            '''
            Loads the tag list from the CondDB
            '''
            if not self.tag_loaded:
                folder = self.listView().bridge.db.getFolder(self.fullName)
                self.tagList = self.listView().bridge.getTagList(self.fullName)
                self.setVersioning(folder.versioningMode())
                self.tag_loaded = True

        def fillFolder(self):
            '''
            If not already done, loads the channels of the folder.
            '''
            if not self.channel_loaded:
                folder = self.listView().bridge.db.getFolder(self.fullName)
                channels = list(folder.listChannels())
                for channelID in channels:
                    guiChannel(self, channelID)
                self.channel_loaded = True

        def setVersioning(self, versioning):
            '''
            Set the version type of the folder and display it in the QListView
            '''
            self.versioning = versioning
            if self.versioning == 0:
                self.setText(1, '  Single')
            elif self.versioning > 0:
                self.setText(1, '  Multi')
            else:
                self.setText(1, '  ?')

class guiFolderSet(guiTreeElement):
    '''
    A FolderSet in the CondDB. It contains CondDB Folders and/or other FolderSets 
    '''
    def __init__(self, parent, name = ''):
        pix = [qt.QPixmap(folderset_closed_xpm), qt.QPixmap(folderset_open_xpm)]
        guiTreeElement.__init__(self, parent, name, pix)
        self.setExpandable(True)

class guiUnknown(guiTreeElement):
    '''
    An empty object simply showing elements for which not enough meaningful
    information is available.
    '''
    def __init__(self, parent, name = ''):
        guiTreeElement.__init__(self, parent, name, None)
