from PyQt4.QtCore import QObject
class SparesInfo(QObject):
    def __init__(self):
        QObject.__init__(self)
        self.spares_in_equipdb = 0
        self.spares_in_confdb = 0
        self.changed_spares = 0
        self.new_spares = 0
    def compute(self):
        pass
    def getSparesInEquipDB(self):
        return self.spares_in_equipdb
    def setSparesInEquipDB(self, value):
        self.spares_in_equipdb = value
    def getSparesInConfDB(self):
        return self.spares_in_confdb
    def setSparesInConfDB(self, value):
        self.spares_in_confdb = value
    def getChangedSpares(self):
        return self.changed_spares
    def setChangedSpares(self, value):
        self.changed_spares = value
    def getNewSpares(self):
        return self.new_spares
    def setNewSpares(self, value):
        self.new_spares = value
    SparesInEquipDB = property(getSparesInEquipDB, setSparesInEquipDB)
    SparesInConfDB = property(getSparesInConfDB, setSparesInConfDB)
    ChangedSpares = property(getChangedSpares, setChangedSpares)
    NewSpares = property(getNewSpares, setNewSpares)