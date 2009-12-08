from PyQt4.QtCore import (QObject, QString,
                          QAbstractItemModel, QAbstractListModel, QAbstractTableModel,
                          QVariant, QModelIndex,
                          Qt, SIGNAL, SLOT)
from PyQt4.QtGui import (QIcon, QApplication, QItemSelectionModel,
                         QItemDelegate,
                         QComboBox, QLineEdit,
                         QBrush, QFont)

from PyCool import cool

from Utils import *

# Import the dictionary for helper functions
import PyCintex
Helpers = PyCintex.gbl.CondDBUI.Helpers

__all__ = ["setModelsIcons",
           "tagsGlobalCache",
           "CondDBNodesListModel",
           "CondDBStructureModel",
           "CondDBTagsListModel",
           "CondDBIoVModel",
           "CondDBPayloadFieldModel",
           "NodeFieldsModel",
           "AddConditionsStackModel",
           "CondDBSelectionsModel",
           "ChildTagDelegate",
           "ChildTagsModel",
           ]
#import logging
#logging.basicConfig(level=logging.INFO)
#_log = logging.getLogger(__name__)

## Class to keep a cache of the tags in the current database
class TagsCache(QObject):
    ## Constructor
    def __init__(self):
        super(TagsCache, self).__init__()
        self.db = None
        self.cache = {}
    ## Invalidate the whole cache and use the new database.
    def setDB(self, db):
        self.db = db
        self.cache = {}
        self._allTags = None
        self.emit(SIGNAL("tagsCacheUpdated()"))
    ## Tell if a path may have tags (i.e. it is a multi-version folder or a folderset)
    def mayHaveTags(self, path):
        if self.db.db.existsFolder(path):
            f = self.db.db.getFolder(path)
            return f.versioningMode() == cool.FolderVersioning.MULTI_VERSION
        else:
            return self.db.db.existsFolderSet(path)
    ## Get the list of tags for a path
    def getTags(self, path):
        if self.db:
            if not path:
                return []
            if path not in self.cache:
                _bc = BusyCursor()
                if self.mayHaveTags(path):
                    self.cache[path] = self.db.getTagList(path)
                else:
                    self.cache[path] = []
        return self.cache.get(path, [])
    ## Get all the known tags in the database
    def getAllTagNames(self):
        if self.db:
            if self._allTags is None:
                tags = set()
                for p in self.db.getAllNodes():
                    tags.update([t.name for t in self.getTags(p)])
                tags = list(tags)
                tags.sort()
                tags.insert(0, "HEAD")
                self._allTags = tags
            return self._allTags
        return []
    ## Invalidate the cache for a single path
    def invalidate(self, path):
        if path in self.cache:
            del self.cache[path]
            self._allTags = None
            qpath = QString(path)
            self.emit(SIGNAL("tagsCacheUpdated(QString)"), qpath)

tagsGlobalCache = TagsCache()

icons = {}

## Function to set the icons used by the models.
def setModelsIcons(dict):
    global icons
    icons = dict

## Class to manage the hierarchy of items in CondDBStructureModel.
class CondDBStructureItem(object):
    ## Constructor.
    #  @param parent: parent item (used for quick navigation)
    #  @param node: COOL node object
    def __init__(self, db, parent, name, node, channel = None):
        self.db = db
        
        self.parent = parent
        self.name = name
        
        self.node = node
        self.channel = channel
        
        self.path = str(node.fullPath())
        
        self._children = None
        
        self.leaf = node.isLeaf()
        if self.leaf:
            self.singleVersion = node.versioningMode() == cool.FolderVersioning.SINGLE_VERSION
        else:
            self.singleVersion = False
        self.index = None
        
    ## Disconnect all the relations between parent and children (needed to allow
    #  Python to free the memory)
    def release(self):
        if self._children:
            for c in self._children:
                c.release()
        self._children = None
        self.parent = None
    
    ## Return the root element of the tree
    def root(self):
        if self.parent is None:
            return self
        return self.parent.root()
    
    @property
    def children(self):
        if self._children is None:
            self._children = []
            if self.channel is not None:
                return self._children # no children for a channel inside a folder
            _bc = BusyCursor()
            if self.leaf:
                # If the COOL node is a Folder, the children are the channels.
                
                # FIXME: I need to get both the list of channels and channels with
                #        names because I cannot extract the list of keys from the map
                #        (I do not know if it is a problem with PyROOT or a missing
                #        dictionary in PyCool).
                channels = Helpers.getChannelsWithNames(self.node)
                if (len(channels) == 1) and (0 in channels) and (not channels[0]):
                    # If we have only the default channel, no need to show it
                    return self._children
                
                for c in channels:
                    self._children.append(CondDBStructureItem(db = self.db,
                                                              parent = self,
                                                              name = channels[c] or str(c),
                                                              node = self.node,
                                                              channel = c))
            else:
                # The children are the sub-nodes
                
                for f in self.node.listFolderSets():
                    node = self.db.db.getFolderSet(f)
                    name = basename(f)
                    self._children.append(CondDBStructureItem(db = self.db,
                                                              parent = self,
                                                              name = name,
                                                              node = node))
                for f in self.node.listFolders():
                    node = self.db.db.getFolder(f)
                    name = basename(f)
                    self._children.append(CondDBStructureItem(db = self.db,
                                                              parent = self,
                                                              name = name,
                                                              node = node))
        return self._children

    # ## Number of columns for the children: 1 if FolderSet, 2 if we have many channels, 0 otherwise.
    # def columnCount(self):
    #     if self.node.isLeaf():
    #         if len(self.channels) > 1:
    #             return 2
    #         return 0
    #     return 1
    ## Number of columns for the children: 1 if FolderSet and Folders, 0 for channels.
    def columnCount(self):
        if self.channel is None:
            return 1
        return 0
            
    ## Number of children.
    def rowCount(self):
        return len(self.children)
    
    ## Function to extract from the item the information to present
    def data(self, role):
        global icons
        if role == Qt.DisplayRole:
            return QVariant(self.name)
        elif role == Qt.ToolTipRole:
            if self.channel is None:
                return QVariant(self.path)
            else:
                return QVariant(str(self.channel))
        elif role == Qt.DecorationRole:
            if self.leaf:
                icon = icons.get("folder", None)
            else:
                icon = icons.get("folderset", None)
            if icon:
                return QVariant(icon)
        return QVariant()


## ItemModel used by the CondDB tree view
#  @todo: Re-implement using hasChildren, fetchMore and canFetchMore. http://doc.trolltech.com/4.4/model-view-model-subclassing.html#lazy-population-of-model-data
class CondDBStructureModel(QAbstractItemModel):
    ## Constructor.
    #  @param db: CondDBUI.CondDB instance to use
    #  @param parent: parent widget
    def __init__(self, db = None, parent = None):
        super(CondDBStructureModel,self).__init__(parent)
        self.root = None
        self.connectDB(db)
    
    ## Set the CondDBUI.CondDB instance to use (triggering a refresh of the caches)
    def connectDB(self, db):
        self.reset()
        if self.root:
            self.root.release()
        self.db = db
        if db:
            self.root = CondDBStructureItem(db = db,
                                            parent = None,
                                            name = "",
                                            node = db.db.getFolderSet("/"))
            self.root.index = QModelIndex()
        else:
            self.root = None
    
    ## Number of columns to reserve for the children of the parent index.
    def columnCount(self, parent):
        # Protection if no database open
        if self.db:
            if parent.isValid():
                return parent.internalPointer().columnCount()
            else:
                return self.root.columnCount()
        return 0
    
    ## Number of children of the parent index.
    def rowCount(self, parent):
        # Protection if no database open
        if self.db:
            if parent.isValid():
                return parent.internalPointer().rowCount()
            else:
                return self.root.rowCount()
        return 0
    
    ## Return the index to the item under parent, at position (row, column).
    def index(self, row, column, parent):
        # Protection if no database open
        if self.db:
            if parent.isValid():
                p = parent.internalPointer()
            else:
                p = self.root
            c = p.children[row]
            if c.index is None:
                c.index = self.createIndex(row, column, c)
            return c.index
        return QModelIndex()

    ## Return the index to the parent of the item pointed to by the given index.
    def parent(self, index):
        # Protection if no database open
        if self.db:
            if index.isValid():
                return index.internalPointer().parent.index
        return QModelIndex()

    ## Returns a QVariant object to represent the item in the view.
    def data(self, index, role):
        if index.isValid():
            return index.internalPointer().data(role)
        return QVariant()

    ## Header for the structure view (not used).
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Name")
        return QVariant()
    
    ## Return the index of the specified path
    def findPath(self, path):
        if len(path) <= 1:
            return self.root.index
        if path[0] == "/":
            # strip first separator ("/")
            path = path[1:]
        item = self.root
        for n in path.split("/"):
            cnames = [c.name for c in item.children]
            try:
                row = cnames.index(n)
                c = item.children[row]
                if not c.index: # This means that the tree was not expanded
                    c.index = self.createIndex(row, 0, c)
                item = c
            except ValueError:
                # sub-node not found
                break
        # At this point item.index is defined
        return item.index


## ListModel for the list of folders/folder-sets present in the database.
#  Used by the "path" combobox.
class CondDBNodesListModel(QAbstractListModel):
    FOLDER = 0x1
    FOLDERSET = 0x2
    ALL = FOLDER | FOLDERSET
    def __init__(self, db = None, parent = None,
                 nodeType = ALL, needRoot = False):
        super(CondDBNodesListModel,self).__init__(parent)
        self._nodes = None
        self.db = None
        self.nodeType = nodeType
        self.needRoot = needRoot
        self.connectDB(db)
        
    ## Set the CondDBUI.CondDB instance to use (triggering a refresh of the caches)
    def connectDB(self, db):
        self.reset()
        self.db = db
        if db:
            # prepare to fill the cache of node names
            self._nodes = None
        else:
            # without db we need a fake empty cache
            self._nodes = []
    
    ## Python property to cache the list of nodes in the database.
    #  It doesn't include "/".
    @property
    def nodes(self):
        if self._nodes is None:
            self._nodes = self.db.getAllNodes()
            if not self.needRoot:
                self._nodes.pop(0) # remove "/" (which is always the first one)
            # if a filtering criterion is defined, we filter the list 
            if self.nodeType == self.FOLDER:
                self._nodes = filter(self.db.db.existsFolder, self._nodes)
            elif self.nodeType == self.FOLDERSET:
                self._nodes = filter(self.db.db.existsFolderSet, self._nodes)
        return self._nodes
    
    ## Returns the number of nodes in the database.
    def rowCount(self, _parent):
        return len(self.nodes)
    
    ## Returns the name of the folder at a given index.
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole:
                return QVariant(self.nodes[index.row()])
        return QVariant()
    
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Name")
        return QVariant()

## Model class to retrieve the available tags in a folder.
class CondDBTagsListModel(QAbstractListModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, path = None, parent = None):
        global tagsGlobalCache
        super(CondDBTagsListModel,self).__init__(parent)
        self._path = path
        self._tags = None
        self._hideAutoTags = True
        self.setPath(path)
        QObject.connect(tagsGlobalCache, SIGNAL("tagsCacheUpdated(QString)"),
                        self._refreshedCachePath)
        QObject.connect(tagsGlobalCache, SIGNAL("tagsCacheUpdated()"),
                        self._refreshedCache)
    
    ## Property hideAutoTags
    def getHideAutoTags(self):
        return self._hideAutoTags
    
    ## Hide the Auto(matic)Tags from the list
    def hideAutoTags(self):
        return self.setHideAutoTags(True)
    
    ## Show the Auto(matic)Tags from the list
    def showAutoTags(self):
        return self.setHideAutoTags(False)
    
    ## Sets the property hideAutoTags.
    def setHideAutoTags(self, value):
        newval = bool(value)
        if self._hideAutoTags != newval:
            self.reset()
            self._tags = None
            self._hideAutoTags = newval

    ## Get the current folder.
    def path(self):
        return self._path
    
    ## Set the folder for which to get the tags.
    def setPath(self, path):
        global tagsGlobalCache
        self.reset()
        if path is not None:
            path = str(path) # Convert to Python string since we may get QString
        self._path = path
        self._tags = None # Invalidate the internal cache
        self.emit(SIGNAL("setViewEnabled(bool)"),
                  bool(path and tagsGlobalCache.db.db.existsFolder(path)))
    
    ## Slot to receive the notification of changes in the cache of tags
    def _refreshedCachePath(self, path):
        if path == self._path:
            self.reset()
            self._tags = None
    
    ## Slot to receive the notification of changes in the cache of tags
    def _refreshedCache(self):
        self.reset()
        self._tags = None

    ## Expanded list of tags.
    #  The information in the list is in for of tuples with (name, path, group_id)
    #  If the hideAutoTags property is set to True, the tags starting with
    #  "_auto_" are excluded from the list.
    def tags(self):
        global tagsGlobalCache
        if self._tags is None:
            tags = []
            group = 0
            for t in tagsGlobalCache.getTags(self._path):
                tags.append((t.name, self._path, group))
                for at in t.getAncestorTags():
                    tags.append((at.name, at.path, group))
                group += 1
            if self._hideAutoTags:
                tags = [t for t in tags if not t[0].startswith("_auto_")]
            self._tags = tags
        return self._tags
    
    ## Number of tags to display.
    def rowCount(self, parent):
        return len(self.tags())
    
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            tag, path, group = self.tags()[index.row()]
            if role == Qt.DisplayRole:
                return QVariant(tag)
            elif role == Qt.ToolTipRole:
                return QVariant(path)
            elif role == Qt.BackgroundRole:
                if group % 2:
                    return QVariant(QBrush(Qt.lightGray))
        return QVariant()
    
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Tag")
        return QVariant()

    ## Find the position of the specified tag in the list of known tags
    def findPosOfTag(self, tag):
        tags = self.tags()
        i = 0
        l = len(tags)
        while i < l and tag != tags[i][0]:
            i += 1
        if i >= l:
            i = -1
        return i

## Model class to retrieve the available tags in a database.
class GlobalTagsListModel(QAbstractListModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, parent = None):
        super(GlobalTagsListModel,self).__init__(parent)
        self.db = None
        ## Tags to display
        self._tags = None
        ## Property controlling the filter on the list of tags
        self._hideLocalTags = True
    
    ## Property hideLocalTags
    def getHideLocalTags(self):
        return self._hideLocalTags
    
    ## Hide the local tags from the list
    def hideLocalTags(self):
        return self.setHideLocalTags(True)
    
    ## Show the local tags from the list
    def showLocalTags(self):
        return self.setHideLocalTags(False)
    
    ## Sets the property hideLocalTags.
    def setHideLocalTags(self, value):
        newval = bool(value)
        if self._hideLocalTags != newval:
            self.reset()
            self._tags = None
            self._hideLocalTags = newval

    ## Expanded list of tags.
    #  If the hideLocalTags property is set to True, the tags not defined in the
    #  root folder-set are excluded from the list.
    def tags(self):
        global tagsGlobalCache
        if self._tags is None:
            # @FIXME: hideLocalTags is currently ignored
            self._tags = tagsGlobalCache.getAllTagNames()
        return self._tags
    
    ## Number of tags to display.
    def rowCount(self, parent):
        return len(self.tags())
    
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole:
                return QVariant(self.tags()[index.row()])
        return QVariant()
    
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Tag")
        return QVariant()

## Base class for functionalities shared by all the models handling IoVs
class BaseIoVModel(QAbstractTableModel):
    __pyqtSignals__ = ("setViewEnabled(bool)",
                       "setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)",
                       #"dataChanged(const QModelIndex&,const QModelIndex&)"
                       )
    ## Constructor
    def __init__(self, parent = None):
        super(BaseIoVModel,self).__init__(parent)
        # Property ShowUTC
        self._showUTC = True
        # Property DisplayFormat
        self._format = None
    ## Value of the property ShowUTC.
    #  If set to True, the string returned for as data for the IoV table is UTC. 
    def showUTC(self):
        return self._showUTC
    ## Set the property ShowUTC.
    #  If set to True, the string returned for as data for the IoV table is UTC. 
    def setShowUTC(self, value):
        if self._showUTC != value:
            self._showUTC = value
            rows, cols = self.rowCount(), self.columnCount()
            if rows and cols:
                # Notify the view that the data has changed.
                self.emit(SIGNAL("dataChanged(QModelIndex,QModelIndex)"),
                          self.index(0,0),
                          self.index(rows-1, cols-1))
    ## Format to use to display the IoV limits in the table.
    def displayFormat(self):
        return self._format
    ## Set the format to use to display the IoV limits in the table.
    def setDisplayFormat(self, format):
        self._format = format
    ## Return the string representation of a validity key 
    def validityKeyToString(self, valkey):
        if valkey == cool.ValidityKeyMax:
            s = "Max"
        else:
            dt = valKeyToDateTime(valkey)
            if not self.showUTC():
                dt = dt.toLocalTime()
            s = dt.toString(self.displayFormat())
        return s

## Model class for the list of IOVs
#  @todo: Re-implement using hasChildren, fetchMore and canFetchMore. http://doc.trolltech.com/4.4/model-view-model-subclassing.html#lazy-population-of-model-data
class CondDBIoVModel(BaseIoVModel):
    __pyqtSignals__ = ("setViewEnabled(bool)",
                       "setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)",
                       #"dataChanged(const QModelIndex&,const QModelIndex&)"
                       )
    ## Position of the field in the tuple used internally
    SINCE = 0
    ## Position of the field in the tuple used internally
    UNTIL = 1
    ## Position of the field in the tuple used internally
    PAYLOAD = 2
    ## Position of the field in the tuple used internally
    INSERTION_TIME = 3
    ## Definition of the HEAD tag
    HEAD = "HEAD"
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, db = None, path = None, channel = None, tag = None, parent = None):
        super(CondDBIoVModel,self).__init__(parent)
        
        # "_since" is the value requested by the user,
        # "_actualSince" is the one in the cache
        self._since = self._actualSince = cool.ValidityKeyMin
        self._until = self._actualUntil = cool.ValidityKeyMax
        
        # initializes internal members
        self._reset()
        
        self.connectDB(db)
        self.setPath(path)
        
    ## Set the CondDBUI.CondDB instance to use (triggering a refresh of the caches)
    def connectDB(self, db):
        if self.db:
            self._reset()
            # The actual logic for enable is (self.db and self._path)
            self.emit(SIGNAL("setViewEnabled(bool)"), False)
        self.db = db
    
    ## Reset internal data, cleaning the cache.
    def _reset(self):
        self._path = None
        self._folder = None
        # default values
        self._channel = 0
        self._tag = self.HEAD
        
        self.db = None
        
        # "_sinceIndex" is the index of the element of the cache corresponding to "_since"
        self._sinceIndex = 0
        # same as for _since*
        self._untilIndex = -1
        
        self._selectedIndex = 0
        
        self._cleanCache()
        
    ## Just clean the cache.
    #  Useful when changing channel id or tag.
    def _cleanCache(self):
        # purge cache
        self._allIoVs = None
    
    ## Value of the property channel.
    def channel(self):
        return self._channel
    
    ## Set the channel.
    def setChannel(self, channel):
        if not channel:
            channel = 0
        if self._channel != channel:
            self._cleanCache()
            self._channel = channel
            self.reset()
    
    ## Return the cool::ChannelSelection object corresponding to the set channel.    
    def channelSelection(self):
        return cool.ChannelSelection(self._channel)
    
    ## Value of the property tag.
    def tag(self):
        return self._tag
    
    ## Get the current tag.
    def setTag(self, tag):
        if not tag:
            tag = self.HEAD
        else:
            tag = str(tag)
        if self._tag != tag:
            self._cleanCache()
            self._tag = tag
            self.reset()
    
    ## Add new IoVs from the folder to the end of the cache.
    def _appendIoVs(self, newUntil):
        # cross check, before going to the DB
        if self._folder and self._actualUntil < newUntil:
            tag = self.tag()
            if tag != self.HEAD:
                tag = self._folder.resolveTag(self.tag())
            objects = self._folder.browseObjects(self._actualUntil, newUntil,
                                                 self.channelSelection(),
                                                 tag)
            for o in objects:
                self._allIoVs.append((o.since(), o.until(),
                                      dict(o.payload()),
                                      o.insertionTime()))
            # set actual limits of the content of the cache
            if self._allIoVs: # we may not have found anything
                self._actualUntil = self._allIoVs[-1][self.UNTIL]
                # this is needed when there was no IoV in the cache
                self._actualSince = self._allIoVs[0][self.SINCE]
            else:
                self._actualUntil = newUntil
        
    ## Add new IoVs from the folder to the begin of the cache.
    def _prependIoVs(self, newSince):
        # cross check, before going to the DB 
        if self._folder and self._actualSince > newSince:
            oldSize = len(self._allIoVs)
            tag = self.tag()
            if tag != self.HEAD:
                tag = self._folder.resolveTag(self.tag())
            # Note: we use "self._actualSince - 1" and not "self._actualSince"
            # because COOL returns also the object that includes the upper limit,
            # but we already have it.
            objects = self._folder.browseObjects(newSince, self._actualSince - 1,
                                                 self.channelSelection(),
                                                 tag)
            tmp = []
            for o in objects:
                # FIXME: probably it could be quicker using a temporary list and append
                tmp.append((o.since(), o.until(),
                            dict(o.payload()),
                            o.insertionTime()))
            self._allIoVs = tmp + self._allIoVs
            # set actual limits of the content of the cache
            if self._allIoVs: # we may not have found anything
                self._actualSince = self._allIoVs[0][self.SINCE]
                # this is needed when there was no IoV in the cache
                self._actualUntil = self._allIoVs[-1][self.UNTIL]
            else:
                self._actualSince = newSince
            # Displace the indexes
            newSize = len(self._allIoVs)
            if oldSize == 0:
                if newSize == 0:
                    deltaSize = 0
                else:
                    deltaSize = newSize - 1 # avoid that the indexes go out of bounds
            else:
                deltaSize = newSize - oldSize
            self._sinceIndex += deltaSize
            self._untilIndex += deltaSize
            self._selectedIndex += deltaSize
            #print "prepend",self._sinceIndex,self._untilIndex,self._selectedIndex
            
    
    ## Get all the IoVs in the cache.
    def allIoVs(self):
        if self._allIoVs is None:
            # Initial retrieval
            self._allIoVs = []
            if self._folder:
                # Looks complicated but it is just a way to re-use the _appenIoVs method
                self._actualUntil = self._since
                self._appendIoVs(self._until)
                if self._allIoVs: # we may not have found anything
                    self._actualSince = self._allIoVs[0][self.SINCE]
                else:
                    self._actualSince = self._since
            # set indexes
            self._sinceIndex = 0
            self._untilIndex = len(self._allIoVs) - 1
            if self._allIoVs:
                #print "allIoVs", self._untilIndex
                self.setSelected(self._untilIndex, True)
        return self._allIoVs
    
    ## Update the indexes corresponding to the first and last element of the cache
    #  for the range (since, until).
    def _updateIndexes(self):
        if self._allIoVs:
            # Make sense only if there is a cache
            old = (self._sinceIndex, self._untilIndex)
            oldSelected = self._selectedIndex
            if self._allIoVs[self._sinceIndex][self.SINCE] >= self._since:
                while (self._allIoVs[self._sinceIndex][self.SINCE] > self._since) \
                      and (self._sinceIndex > 0):
                    self._sinceIndex -= 1
            else:
                while (self._allIoVs[self._sinceIndex][self.UNTIL] < self._since) \
                      and (self._sinceIndex < self._untilIndex):
                    self._sinceIndex += 1
            if self._allIoVs[self._untilIndex][self.UNTIL] <= self._until:
                last = len(self._allIoVs) - 1
                while (self._allIoVs[self._untilIndex][self.UNTIL] < self._until) \
                      and (self._untilIndex < last):
                    self._untilIndex += 1
            else:
                while (self._allIoVs[self._untilIndex][self.SINCE] > self._until) \
                      and (self._untilIndex > self._sinceIndex):
                    self._untilIndex -= 1
            if old != (self._sinceIndex, self._untilIndex):
                # we have to move the selected index to match the displacement
                self.setSelected(min(self._sinceIndex - old[0] + oldSelected, self._untilIndex))
                # notify view
                # FIXME: Should be done better
                self.reset()
    
    ## Value of the property since.
    def since(self):
        return self._since

    ## Set the property since updating the cache if needed.
    def setSince(self, since):
        if since > self._until:
            # FIXME: should we exit or set since to the value of until?
            return
        self._since = since
        if self._allIoVs is not None:
            # Fetch data if needed
            self._prependIoVs(since)
            self._updateIndexes()

    ## Value of the property until.
    def until(self):
        return self._until

    ## Set the property until updating the cache if needed.
    def setUntil(self, until):
        if until < self._since:
            # FIXME: should we exit or set until to the value of since?
            return
        self._until = until
        if self._allIoVs is not None:
            # the complex part make sense only if we have something in the cache
            # Fetch data if needed
            self._appendIoVs(until)
            self._updateIndexes()

    ## Get the path of the current folder.
    def path(self):
        return self._path
    
    ## Set the folder for which to get the tags.
    def setPath(self, path):
        if path != self._path:
            self._cleanCache()
            self._path = self._folder = None
            self._tag = self.HEAD # To avoid that we pick up an old tag (coming from another path)
            if path and self.db.db.existsFolder(path):
                self._path = path
                self._folder = self.db.db.getFolder(path)
            # Notify the views
            self.reset()
            # The actual logic for enable is (self.db and self._path)
            self.emit(SIGNAL("setViewEnabled(bool)"), bool(self._path))
    
    ## Set the path and the channel.
    def setPathChannel(self, path, channel):
        self.setPath(path)
        self.setChannel(channel)
        
    ## Number of IOV in the range.
    def rowCount(self, parent = None):
        self.allIoVs() # trigger the filling of the cache
        return self._untilIndex - self._sinceIndex + 1
    
    ## Number of columns in the table. 
    def columnCount(self, parent = None):
        return 2
    
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            data = self.allIoVs()[self._sinceIndex + index.row()]
            if role == Qt.DisplayRole:
                valkey = data[index.column()]
                return QVariant(self.validityKeyToString(valkey))
            elif role == Qt.ToolTipRole:
                s = "Insertion time: %s" % data[self.INSERTION_TIME]
                return QVariant(s)
        return QVariant()

    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant(("Since", "Until")[section])
            else:
                return QVariant(section)
        return QVariant()

    def setSelectedTime(self, validityKey):
        if self._allIoVs:
            idx = self._sinceIndex
            while idx < self._untilIndex and \
                  self._allIoVs[idx][self.UNTIL] < validityKey:
                idx += 1
            self.setSelected(idx - self._sinceIndex)

    ## Get the row number of the currently selected IoV
    def selected(self):
        return self._selectedIndex

    ## Set the current selection.
    #  Emit the setCurrentIndex signal if the parameter "emit" is set to True.
    def setSelected(self, index, emit = True):
        if self._allIoVs:
            self._selectedIndex = index
            if emit:
                self.emit(SIGNAL("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"),
                          self.index(self._selectedIndex, 0),
                          QItemSelectionModel.ClearAndSelect | QItemSelectionModel.Rows)

    ## Slot used to notify the model that the selection in the view has changed. 
    def selectionChanged(self, index, oldIndex):
        self.setSelected(index.row(), False)
    
    ## Get the payload of the currently selected IoV.
    def getPayload(self):
        if self._allIoVs and self._selectedIndex is not None:
            return self._allIoVs[self._selectedIndex + self._sinceIndex][self.PAYLOAD]
        return None

## Model class to retrieve the available fields in a folder.
class CondDBPayloadFieldModel(QAbstractListModel):
    __pyqtSignals__ = ("setViewEnabled(bool)",
                       "setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)")
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, db = None, path = None, parent = None):
        super(CondDBPayloadFieldModel, self).__init__(parent)
        self.db = None
        self._path = None
        self._fields = []
        self._selected = 0
        
        self.connectDB(db)
        self.setPath(path)
    
    ## Set the CondDBUI.CondDB instance to use (triggering a refresh of the caches)
    def connectDB(self, db):
        self.setPath(None) # trigger a clean up of the cache
        self.db = db
    
    ## Get the current folder.
    def path(self):
        return self._path
    
    ## Set the folder for which to get the tags.
    def setPath(self, path):
        self.reset()
        if path is not None: # Needed to use this function as a slot accepting QString
            path = str(path)
        self._path = path
        if path and self.db.db.existsFolder(path):
            self._fields = self.db.getFolderStorageKeys(path)
            self._fields.sort()
            viewEnabled = len(self._fields) != 1
            self.emit(SIGNAL("setViewEnabled(bool)"), viewEnabled)
            self.setSelectedField(0)
        else:
            # If no folder is specified or the path is a folderset, use an empty cache
            self._fields = []
            self.emit(SIGNAL("setViewEnabled(bool)"), False)
    
    ## Number of tags to display.
    def rowCount(self, parent):
        return len(self._fields)
    
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole:
                return QVariant(self._fields[index.row()])
        return QVariant()
    
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Name")
        return QVariant()
    
    ## Set the current selection.
    #  Emit the setCurrentIndex signal if the parameter "emit" is set to True.
    def setSelectedField(self, row, emit = True):
        self._selected = row
        if emit:
            self.emit(SIGNAL("setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)"),
                      self.index(self._selected), QItemSelectionModel.ClearAndSelect)
    
    ## Slot used to notify the model that the selection in the view has changed. 
    def selectionChanged(self, index, oldIndex):
        self.setSelectedField(index.row(), False)

    ## Get the name of the currently selected field.
    def getFieldName(self):
        if self._fields:
            return self._fields[self._selected]
        return None

## List of supported COOL storage types
__cool_storage_types__ = [
                          "Bool",
                          # "Char",   # not (yet?) supported
                          "UChar",
                          "Int16",
                          "UInt16",
                          "Int32",
                          "UInt32",
                          "UInt63",
                          "Int64",
                          # "UInt64", # not (yet?) supported
                          "Float",
                          "Double",
                          "String255",
                          "String4k",
                          "String64k",
                          "String16M",
                          "Blob64k",
                          "Blob16M",
                          ]
__default_field__ = ("data", "String16M")

class NodeFieldsDelegate(QItemDelegate):
    def __init__(self, parent):
        super(NodeFieldsDelegate, self).__init__(parent)
    def createEditor(self, parent, option, index):
        if index.column() == 1:
            editor = QComboBox(parent)
            editor.addItems(__cool_storage_types__)
        else:
            editor = QLineEdit(parent)
        return editor
    def setEditorData(self, editor, index):
        value = index.model().data(index, Qt.EditRole)
        if index.column() == 1:
            editor.setCurrentIndex(__cool_storage_types__.index(str(value.toString())))
        else:
            editor.setText(value.toString())
    def setModelData(self, editor, model, index):
        if index.column() == 1:
            value = editor.currentText()
        else:
            value = editor.text()
        model.setData(index, QVariant(value), Qt.EditRole)
    def updateEditorGeometry(self, editor, option, index):
        editor.setGeometry(option.rect)

## Class for the management of the list of fields used to create a new node.
class NodeFieldsModel(QAbstractTableModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, parent = None):
        super(NodeFieldsModel,self).__init__(parent)
        self.fields = [__default_field__]
    ## Number of fields in the range.
    def rowCount(self, parent = None):
        return len(self.fields)
    ## Number of columns in the table. 
    def columnCount(self, parent = None):
        return 2
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole or role == Qt.EditRole:
                return QVariant(self.fields[index.row()][index.column()])
            elif role == Qt.ToolTipRole:
                return QVariant()
        return QVariant()
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant(("Name", "Type")[section])
            else:
                return QVariant()
        return QVariant()
    ## Flags for the item (to make the item editable)
    def flags(self, index):
        if not index.isValid():
            return Qt.ItemIsEnabled
        return QAbstractTableModel.flags(self, index) | Qt.ItemIsEditable
    ## Handle the changes in the data
    def setData(self, index, value, role):
        if (index.isValid() and role == Qt.EditRole):
            data = list(self.fields[index.row()])
            data[index.column()] = str(value.toString())
            self.fields[index.row()] = tuple(data)
            self.emit(SIGNAL("dataChanged(QModelIndex,QModelIndex)"), index, index)
            return True
        return False
    def insertRow(self, position, parent = QModelIndex()):
        return self.insertRows(position, 1, parent)
    def insertRows(self, position, rows, parent = QModelIndex()):
        self.beginInsertRows(QModelIndex(), position, position+rows-1)
        # find the highest number used in a field with name "field_%d"
        # so that the new fields will start for that id +1 to avoid conflicts
        id = -1
        for n,_t in self.fields:
            if n.startswith("field_"):
                try:
                    nid = int(n[6:])
                    if nid > id:
                        id = nid
                except: # ignore conversion errors
                    pass
        id += 1
        count = rows
        while count > 0:
            count -= 1
            self.fields.insert(position, ("field_%d" % (id + count),
                                          __default_field__[1]))
        self.endInsertRows()
        return True
    def removeRow(self, position, parent = QModelIndex()):
        return self.removeRows(position, 1, parent)
    def removeRows(self, position, rows, parent = QModelIndex()):
        self.beginRemoveRows(QModelIndex(), position, position+rows-1)
        del self.fields[position:position+rows]
        self.endRemoveRows()
        return True

## Simple class to hold the data in the AddConditionsStackModel.
class ConditionStackItem(object):
    ## Initialize
    def __init__(self, since, until, channel, data):
        self.since = since
        self.until = until
        if not channel:
            channel = 0
        self.channel = channel
        self.data = dict(data)

## Class for the management of the list conditions in the AddConditions dialog.
class AddConditionsStackModel(BaseIoVModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, parent = None):
        super(AddConditionsStackModel, self).__init__(parent)
        self.conditions = []
        # list of conditions (indexes) with conflicts
        self.conflicts = set()
    ## Number of fields in the range.
    def rowCount(self, parent = None):
        return len(self.conditions)
    ## Number of columns in the table. 
    def columnCount(self, parent = None):
        return 3
    ## Get one of the conditions in the stack
    def __getitem__(self, item):
        return self.conditions[item]
    ## iterate on the conditions of the stack
    def __iter__(self):
        return self.conditions.__iter__()
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            r = index.row()
            if role == Qt.DisplayRole:
                c = index.column()
                cond = self.conditions[r]
                if c == 2:
                    return QVariant(str(cond.channel))
                elif c == 0:
                    valkey = cond.since
                else:
                    valkey = cond.until
                return QVariant(self.validityKeyToString(valkey))
            elif r in self.conflicts:
                # Visual feedback for conflicts
                if role == Qt.ToolTipRole:
                    return QVariant("This Interval of Validity is overlapping with another.")
                elif role == Qt.BackgroundRole:
                    return QVariant(QBrush(Qt.red))
                elif role == Qt.FontRole:
                    font = QFont()
                    font.setItalic(True)
                    return QVariant(font)
        return QVariant()
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant(("Since", "Until", "Channel")[section])
            else:
                return QVariant()
        return QVariant()
    ## Check for conflicts and fill the conflicts list:
    def _checkConflicts(self):
        count = len(self.conditions)
        new_conflicts = set()
        for i in range(count-1):
            ci = self.conditions[i]
            for j in range(i+1, count):
                cj = self.conditions[j]
                # There is an overlap if the intersection of two intervals is not empty
                s = max(ci.since, cj.since)
                u = min(ci.until, cj.until)
                if (ci.channel == cj.channel) and (u > s):
                    new_conflicts.add(i)
                    new_conflicts.add(j)
        result = bool(new_conflicts)
        self.emit(SIGNAL("conflictsChanged(bool)"), result)
        if new_conflicts != self.conflicts:
            #self.emit(SIGNAL("layoutAboutToBeChanged()"))
            self.conflicts = new_conflicts
            #self.emit(SIGNAL("layoutChanged()"))
        return result
    ## Add a new condition object to the stack
    def addCondition(self, since, until, channel, data):
        condition = ConditionStackItem(since = since, until = until,
                                       channel = channel, data = data)
        position = self.rowCount()
        self.beginInsertRows(QModelIndex(), position, position)
        self.conditions.append(condition)
        self._checkConflicts()
        self.endInsertRows()
    ## Remove a condition object from the stack
    def removeCondition(self, position):
        self.beginRemoveRows(QModelIndex(), position, position)
        del self.conditions[position]
        self._checkConflicts()
        self.endRemoveRows()
    ## Remove all the conditions in the selection (a QModelIndexList).
    def removeConditions(self, selection):
        positions = list(set([i.row() for i in selection]))
        positions.sort()
        if positions:
            self.beginRemoveRows(QModelIndex(), positions[0], positions[-1])
            while positions:
                del self.conditions[positions.pop()]
            self._checkConflicts()
            self.endRemoveRows()
    def moveUp(self, row):
        if row > 0 and row < len(self.conditions):
            self.emit(SIGNAL("layoutAboutToBeChanged()"))
            self.conditions.insert(row-1, self.conditions.pop(row))
            self.emit(SIGNAL("layoutChanged()"))
    def moveDown(self, row):
        if row >= 0 and row < (len(self.conditions)-1):
            self.emit(SIGNAL("layoutAboutToBeChanged()"))
            self.conditions.insert(row+1, self.conditions.pop(row))
            self.emit(SIGNAL("layoutChanged()"))

## Model for the list of selections for the CondDB slice
class CondDBSelectionsModel(BaseIoVModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, parent = None):
        super(CondDBSelectionsModel, self).__init__(parent)
        self.selections = []
    ## Number of fields in the range.
    def rowCount(self, parent = None):
        return len(self.selections)
    ## Number of columns in the table. 
    def columnCount(self, parent = None):
        return 4
    ## Get one of the conditions in the stack
    def __getitem__(self, item):
        return self.selections[item]
    ## iterate on the conditions of the stack
    def __iter__(self):
        return self.selections.__iter__()
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            r = index.row()
            if role == Qt.DisplayRole:
                c = index.column()
                s = self.selections[r]
                if c in [0, 3]:
                    return QVariant(str(s[c]))
                else:
                    return QVariant(self.validityKeyToString(s[c]))
        return QVariant()
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant(("Path", "Since", "Until", "Tags")[section])
            else:
                return QVariant()
        return QVariant()
    ## Add a new condition object to the stack
    def addSelection(self, path, since, until, tags):
        # Search where to insert the selection in the list
        i = 0
        count = len(self.selections)
        match = False
        while i < count and self.selections[i][0] <= path:
            if (self.selections[i][1:3] == (since, until) and
                self.selections[i][0] == path):
                match = True
                break
            i += 1
        if not match:
            # we need to do a real insert
            self.beginInsertRows(QModelIndex(), i, i)
            tags = list(tags)
            tags.sort()
            self.selections.insert(i,(path, since, until, tags))
            self.endInsertRows()
        else:
            # we already have the path+IoV, let's just update the tags
            tags = list(set(tags + self.selections[i][3]))
            tags.sort()
            self.selections[i] = (path, since, until, tags)
            self.emit(SIGNAL("dataChanged(QModelIndex,QModelIndex)"),
                      self.index(i,0), self.index(i,3))
    ## Remove a condition object from the stack
    def removeSelection(self, row):
        self.beginRemoveRows(QModelIndex(), row, row)
        del self.selections[row]
        self.endRemoveRows()


## QItemDelegate to select the tag for a child tag
class ChildTagDelegate(QItemDelegate):
    def __init__(self, parent, root):
        super(ChildTagDelegate, self).__init__(parent)
        self.root = str(root)
    def createEditor(self, parent, option, index):
        path = self.root + "/" + str(index.model().key(index.row()))
        editor = QComboBox(parent)
        editor.setModel(CondDBTagsListModel(path, editor))
        return editor
    def setEditorData(self, editor, index):
        value = str(index.model().data(index, Qt.EditRole).toString())
        editor.setCurrentIndex(editor.model().findPosOfTag(value))
    def setModelData(self, editor, model, index):
        value = editor.currentText()
        model.setData(index, QVariant(value), Qt.EditRole)
    def updateEditorGeometry(self, editor, option, index):
        editor.setGeometry(option.rect)

## Handle the list of nodes and tags.
#  Poor-man implementation of an ordered map
class ChildTagsModel(QAbstractTableModel):
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, db, path, parent = None):
        super(ChildTagsModel, self).__init__(parent)
        self._data = []
        self._db = db
        if path == "/":
            self._path = ""
        else:
            self._path = path
        for n in db.getChildNodes(path):
            n = n.rsplit("/", 1)[-1]
            self._data.append((n, "HEAD"))
    def setFromParentTag(self, tag):
        newData = []
        for n, _t in self._data:
            path = self._path + "/" + n
            try:
                f = self._db.getCOOLNode(path)
                f.resolveTag(tag)
                newData.append((n, tag))
            except:
                newData.append((n, "HEAD"))
        self._data = newData
        self.reset()
    ## Mapping interface: __len__
    def __len__(self):
        return len(self._data)
    ## Mapping interface: __getitem__
    def __getitem__(self, key):
        for k, v in self._data:
            if k == key:
                return v
        raise KeyError, key
    ## Mapping interface: __setitem__
    def __setitem__(self, key, value):
        i = 0
        l = self.__len__()
        while i < l and key < self._data[i][0]:
            i += 1
        if i < l and key == self._data[i][0]:
            self._data[i] = (key, value)
        else:
            self._data.insert(i, (key, value))
    ## Mapping interface: __delitem__
    def __delitem__(self, key):
        i = 0
        l = self.__len__()
        while i < l and key < self._data[i][0]:
            i += 1
        if i < l and key == self._data[i][0]:
            del self._data[i]
        else:
            raise KeyError, key
    def key(self, i):
        return self._data[i][0]
    def value(self, i):
        return self._data[i][1]
    def keys(self):
        return [i[0] for i in self._data]
    def values(self):
        return [i[1] for i in self._data]
    def items(self):
        return self._data
    def iter(self):
        return self.__iter__()
    def __iter__(self):
        return self.keys().__iter__()
    ## Number of entries in the range.
    def rowCount(self, parent = None):
        return len(self)
    ## Number of columns in the table. 
    def columnCount(self, parent = None):
        return 2
    ## Name of the tag at a given index.
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole or role == Qt.EditRole:
                return QVariant(self._data[index.row()][index.column()])
            elif role == Qt.ToolTipRole:
                return QVariant()
        return QVariant()
    ## Header for the view (not used).
    def headerData(self, section, orientation ,role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return QVariant(("Child", "Tag")[section])
            else:
                return QVariant()
        return QVariant()
    ## Flags for the item (to make the item editable)
    def flags(self, index):
        if not index.isValid():
            return Qt.ItemIsEnabled
        flags = QAbstractTableModel.flags(self, index)
        if index.column() == 1:
            flags |= Qt.ItemIsEditable
        return flags
    ## Handle the changes in the data
    def setData(self, index, value, role):
        if (index.isValid() and role == Qt.EditRole):
            i = index.row()
            self._data[i] = (self._data[i][0], str(value.toString()))
            self.emit(SIGNAL("dataChanged(QModelIndex,QModelIndex)"), index, index)
            return True
        return False
