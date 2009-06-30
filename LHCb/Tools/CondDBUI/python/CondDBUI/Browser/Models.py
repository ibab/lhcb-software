from PyQt4.QtCore import (QAbstractItemModel, QAbstractListModel, QAbstractTableModel,
                          QVariant, QModelIndex,
                          QDateTime, QDate, QTime,
                          Qt, SIGNAL, SLOT)
from PyQt4.QtGui import QIcon, QApplication, QItemSelectionModel

from PyCool import cool

import time

__all__ = ["CondDBNodesListModel",
           "CondDBStructureModel",
           "CondDBTagsListModel",
           "CondDBIoVModel",
           "CondDBPayloadFieldModel",
           "setModelsIcons"]
#import logging
#logging.basicConfig(level=logging.INFO)
#_log = logging.getLogger(__name__)

#def _indexName(index):
#    name = None
#    if index.isValid():
#        name = index.internalPointer()
#    return name

# Number of times to indent output
# A list is used to force access by reference
__report_indent = [0]
def report(fn):
    """Decorator to print information about a function
    call for use while debugging.
    Prints function name, arguments, and call number
    when the function is called. Prints this information
    again along with the return value when the function
    returns.
    """
    def wrap(*params,**kwargs):
        call = wrap.callcount = wrap.callcount + 1
        indent = ' ' * __report_indent[0]
        fc = "%s(%s)" % (fn.__name__, ', '.join(
            [a.__repr__() for a in params] +
            ["%s = %s" % (a, repr(b)) for a,b in kwargs.items()]
        ))
        print "%s%s called [#%s]" % (indent, fc, call)
        __report_indent[0] += 1
        ret = fn(*params,**kwargs)
        __report_indent[0] -= 1
        print "%s%s returned %s [#%s]" % (indent, fc, repr(ret), call)
        return ret
    wrap.callcount = 0
    return wrap

def report_(fn):
    """Decorator to print information about a function
    call for use while debugging.
    Prints function name, arguments, and call number
    when the function is called. Prints this information
    again along with the return value when the function
    returns.
    """
    def wrap(*params,**kwargs):
        #call = wrap.callcount = wrap.callcount + 1
        #indent = ' ' * __report_indent[0]
        #fc = "%s(%s)" % (fn.__name__, ', '.join(
        #    [a.__repr__() for a in params] +
        #    ["%s = %s" % (a, repr(b)) for a,b in kwargs.items()]
        #))
        #print "%s%s called [#%s]" % (indent, fc, call)
        #__report_indent[0] += 1
        ret = fn(*params,**kwargs)
        #__report_indent[0] -= 1
        #print "%s%s returned %s [#%s]" % (indent, fc, repr(ret), call)
        return ret
    #wrap.callcount = 0
    return wrap


icons = {}

## Function to set the icons used by the models.
def setModelsIcons(dict):
    global icons
    icons = dict

## Utility function to extract the basename from a path
def basename(path):
    if len(path) > 1:
        path = path.split("/")[-1]
    return path
## Utility function to extract the name of the parent node from a path
def parentpath(path):
    if len(path) == 1: # root node
        return None
    parent = path.rsplit("/",1)[0]
    if not parent:
        parent = "/"
    return parent

## Guard-like class to change the cursor icon during operations that may take a
#  long time.
class BusyCursor(object):
    ## Constructor, sets the application cursor to Qt.WaitCursor.
    def __init__(self):
        QApplication.setOverrideCursor(Qt.WaitCursor)
    ## Destructor, restore the application cursor.
    def __del__(self):
        QApplication.restoreOverrideCursor()

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
        return parent.root()
    
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
                channels = self.node.listChannels()
                names = self.node.listChannelsWithNames()
                if len(channels) == 1 and channels[0] == 0 and not names[0]:
                    # If we have only the default channel, no need to show it
                    return self._children
                
                for c in channels:
                    self._children.append(CondDBStructureItem(db = self.db,
                                                              parent = self,
                                                              name = str(names[c]) or str(c),
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
    def __init__(self, db = None, parent = None):
        super(CondDBNodesListModel,self).__init__(parent)
        self._nodes = None
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
            self._nodes.pop(0) # remove "/" (which is always the first one)
        return self._nodes
    
    ## Returns the number of nodes in the database.
    def rowCount(self, parent):
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
    __pyqtSignals__ = ("setViewEnabled(bool)")
    ## Constructor.
    #  Initializes some internal data.
    def __init__(self, db = None, path = None, parent = None):
        super(CondDBTagsListModel,self).__init__(parent)
        self.db = None
        self._path = None
        self._alltags = {}
        self._tags = None
        self._hideAutoTags = True
        
        self.connectDB(db)
        self.setPath(path)
    
    ## Set the CondDBUI.CondDB instance to use (triggering a refresh of the caches)
    def connectDB(self, db):
        self._alltags = {}
        self.setPath(None) # trigger a clean up of the cache
        self.db = db
        
    ## Property showAutoTags
    def hideAutoTags(self):
        return self._hideAutoTags
    
    ## Sets the property showAutoTags.
    def setHideAutoTags(self, value):
        newval = value != 0
        if self._hideAutoTags != newval:
            self.reset()
            self._tags = None
            self._hideAutoTags = newval

    ## Get the current folder.
    def path(self):
        return self._path
    
    ## Set the folder for which to get the tags.
    def setPath(self, path):
        self.reset()
        self._path = path
        if path and self.db.db.existsFolder(path):
            f = self.db.db.getFolder(path)
            if f.versioningMode() == cool.FolderVersioning.MULTI_VERSION:
                # invalidate the cache
                self._tags = None
                if path not in self._alltags:
                    # Initialize the all-tag list (the already retrieved ones are not touched)
                    self._alltags[path] = None
                self.emit(SIGNAL("setViewEnabled(bool)"), True)
            else:
                # single version folders do not have tags by definition
                self._tags = self._alltags[path] = []
                self.emit(SIGNAL("setViewEnabled(bool)"), False)
        else:
            # If no folder is specified or the path is a folderset, use an empty cache
            self._tags = self._alltags[path] = []
            self.emit(SIGNAL("setViewEnabled(bool)"), False)
    
    ## Function returning the (cached) list of all tags.
    def alltags(self):
        if self._alltags[self._path] is None:
            bc = BusyCursor()
            self._alltags[self._path] = self.db.getTagList(self._path)
        return self._alltags[self._path]
    
    ## Expanded list of tags.
    #  If the hideAutoTags property is set to True, the tags starting with
    #  "_auto_" are excluded from the list.
    def tags(self):
        if self._tags is None:
            tags = []
            for t in self.alltags():
                tags.append(t.name)
                tags += t.getAncestors()
            if self._hideAutoTags:
                tags = [t for t in tags if not t.startswith("_auto_")]
            self._tags = tags
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


## Helper function to convert a cool::ValidityKey to a QDateTime (UTC).
def _valKeyToDateTime(valkey):
    # Cannot use setTime_t because of the limited range.
    timeTuple = time.gmtime(valkey / 1e9)
    d = apply(QDate, timeTuple[0:3])
    t = apply(QTime, timeTuple[3:6])
    return QDateTime(d, t, Qt.UTC)
    
## Model class for the list of IOVs
class CondDBIoVModel(QAbstractTableModel):
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
        
        # Property ShowUTC
        self._showUTC = True
        
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
                if valkey == cool.ValidityKeyMax:
                    s = "Max"
                else:
                    dt = _valKeyToDateTime(valkey)
                    if not self.showUTC():
                        dt = dt.toLocalTime()
                    s = dt.toString(self.displayFormat())
                return QVariant(s)
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
    
    ## Value of the property ShowUTC.
    #  If set to True, the string returned for as data for the IoV table is UTC. 
    def showUTC(self):
        return self._showUTC
    
    ## Set the property ShowUTC.
    #  If set to True, the string returned for as data for the IoV table is UTC. 
    def setShowUTC(self, value):
        if self._showUTC != value:
            rows, cols = self.rowCount(), self.columnCount()
            if rows and cols:
                # Notify the view that the data has changed.
                self.emit(SIGNAL("dataChanged(QModelIndex,QModelIndex)"),
                          self.index(0,0),
                          self.index(rows-1, cols-1))
        self._showUTC = value

    ## Format to use to display the IoV limits in the table.
    def displayFormat(self):
        return self._format
    
    ## Set the format to use to display the IoV limits in the table.
    def setDisplayFormat(self, format):
        self._format = format

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