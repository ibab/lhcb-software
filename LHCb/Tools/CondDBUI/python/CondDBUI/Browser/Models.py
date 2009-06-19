from PyQt4.QtCore import (QAbstractItemModel, QAbstractListModel,
                          QVariant, QModelIndex,
                          Qt, SIGNAL, SLOT)
from PyQt4.QtGui import QIcon, QApplication

from PyCool import cool

__all__ = ["CondDBNodesListModel",
           "CondDBStructureModel",
           "CondDBTagsListModel",
           "setModelsIcons"]
#import logging
#logging.basicConfig(level=logging.INFO)
#_log = logging.getLogger(__name__)

#def _indexName(index):
#    name = None
#    if index.isValid():
#        name = index.internalPointer()
#    return name

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

    ## Header for the structure view
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

class CondDBNodesListModel(QAbstractListModel):
    def __init__(self, db = None, parent = None):
        super(CondDBNodesListModel,self).__init__(parent)
        self.db = db
        self._nodes = None
        
    def connectDB(self, db):
        self.reset()
        self.db = db
        if db:
            # prepare to fill the cache of node names
            self._nodes = None
        else:
            # without db we need a fake empty cache
            self._nodes = []
    
    @property
    def nodes(self):
        if self._nodes is None:
            self._nodes = self.db.getAllNodes()
            self._nodes.pop(0) # remove "/" (which is always the first one)
        return self._nodes
    
    def rowCount(self, parent):
        if not self.db:
            return 0
        return len(self.nodes)
    
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole:
                return QVariant(self.nodes[index.row()])
        return QVariant()
    
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Name")
        return QVariant()        

## Model class to retrieve the available tags in a folder.
class CondDBTagsListModel(QAbstractListModel):
    __pyqtSignals__ = ("setViewEnabled(bool)")
    
    def __init__(self, db = None, path = None, parent = None):
        super(CondDBTagsListModel,self).__init__(parent)
        self.db = None
        self._path = None
        self._alltags = None
        self._tags = None
        self._hideAutoTags = True
        
        self.connectDB(db)
        self.setPath(path)
        
    def connectDB(self, db):
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
                self._alltags = None
                self.emit(SIGNAL("setViewEnabled(bool)"), True)
            else:
                # single version folders do not have tags by definition
                self._tags = self._alltags = []
                self.emit(SIGNAL("setViewEnabled(bool)"), False)
        else:
            # If no folder is specified or the path is a folderset, use an empty cache
            self._tags = self._alltags = []
            self.emit(SIGNAL("setViewEnabled(bool)"), False)
    
    def alltags(self):
        if self._alltags is None:
            bc = BusyCursor()
            self._alltags = self.db.getTagList(self._path)
        return self._alltags
    
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
    
    def rowCount(self, parent):
        return len(self.tags())
    
    def data(self, index, role):
        if index.isValid():
            if role == Qt.DisplayRole:
                return QVariant(self.tags()[index.row()])
        return QVariant()
    
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Tag")
        return QVariant()        
