from PyQt4.QtCore import QAbstractItemModel, QVariant, QModelIndex, Qt
from PyQt4.QtGui import QIcon

#import logging
#logging.basicConfig(level=logging.INFO)
#_log = logging.getLogger(__name__)

#def _indexName(index):
#    name = None
#    if index.isValid():
#        name = index.internalPointer()
#    return name

class CondDBStructureModelItem(object):
    def __init__(self, node):
        self.node = node
        self._rows = None
        self._columns = None
        self.index = None

    @property
    def rows(self):
        if self._rows is None:
            if self.node.isLeaf():
                self._rows = []
            else:
                self._rows = map(str, self.node.listFolderSets()) + map(str, self.node.listFolders())
        return self._rows
    
    @property
    def columns(self):
        if self._columns is None:
            if self.node.isLeaf():
                self._columns = []
            else:
                self._columns = [ "child" ] # dummy content
        return self._columns

    @property
    def path(self):
        return self.node.fullPath()

    @property
    def basename(self):
        p = str(self.path)
        if len(p) > 1:
            p = p.split("/")[-1]
        return p
    
    @property
    def parentpath(self):
        path = self.path
        if len(path) == 1: # root node
            return None
        parent = path.rsplit("/",1)[0]
        if not parent:
            parent = "/"
        return parent

    def __str__(self):
        return self.path

class EmptyCondDBStructureModelItem(object):
    def __init__(self):
        self.node = None
        self.rows = []
        self.columns = []
        self.path = ""
        self.basename = ""
        self.parentpath = ""
        self.__str__ = lambda self: ""
        self.index = QModelIndex()

class CondDBStructureModel(QAbstractItemModel):
    def __init__(self, db = None, parent = None):
        super(CondDBStructureModel,self).__init__(parent)
        self.db = db
        self._items = {}
        self.icons = {}

    def connectDB(self, db):
        self.reset()
        self._items = {}
        self.db = db

    def _getItem(self, path):
        if not self.db:
            return EmptyCondDBStructureModelItem()
        if type(path) is QModelIndex:
            if path.isValid():
                path = path.internalPointer()
            else:
                path = "/"
        item = self._items.get(path, None)
        if item is None:
            node = self.db.getCOOLNode(path)
            item = CondDBStructureModelItem(node)
            self._items[path] = item
        return item

    def index(self, row, column, parent):
        parentitem = self._getItem(parent)
        try:
            if column >= len(parentitem.columns):
                return QModelIndex()
            path = parentitem.rows[row]
            item = self._getItem(path)
            if not item.index:
                item.index = self.createIndex(row, column, path)
            return item.index
        except IndexError:
            return QModelIndex()
        
    def parent(self, path):
        item = self._getItem(path)
        parentitem = self._getItem(item.parentpath)
        if not parentitem.index:
            return QModelIndex()
        return parentitem.index
        
    def columnCount(self, parent):
        return len(self._getItem(parent).columns)
    
    def rowCount(self, parent):
        return len(self._getItem(parent).rows)

    def data(self, index, role):
        item = self._getItem(index)
        if role == Qt.DisplayRole:
            return QVariant(item.basename)
        elif role == Qt.ToolTipRole:
            return QVariant(item.path)
        elif role == Qt.DecorationRole:
            if item.node.isLeaf():
                icon = self.icons.get("folder", None)
            else:
                icon = self.icons.get("folderset", None)
            if icon:
                return QVariant(icon)
        return QVariant()
    
    def headerData(self, section, orientation ,role):
        if orientation == Qt.Horizontal and role == Qt.DisplayRole and section == 0:
            return QVariant("Name")
        return QVariant()
