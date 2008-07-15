from PyQt4 import QtCore
from view.mainwindow import MainWindow
from controller import Controller
from newdeviceswindowcontroller import NewDevicesWindowController
from deletedevicescontroller import DeleteDevicesController
from model.devicesinfo import DevicesInfo
from model.sparesinfo import SparesInfo
from view.deviceswidget import DevicesWidget
from view.spareswidget import SparesWidget
from database.spare import SpareDB
from worker.RefreshWorker import RefreshWorker

class MainWindowController(Controller):
    def __init__(self, application):
        Controller.__init__(self)
        self.application = application
        self.spareDB = SpareDB(self.getConfDBCxOracleConnection(), self.getEquipDBCxOracleConnection(), self.logfile)
        self.devicesInfo = self.getDevicesInfo()
        self.sparesInfo = self.getSparesInfo()
        self.mainWindow = MainWindow(self)
        self.devicesWidget = DevicesWidget(self.devicesInfo, self.mainWindow)
        self.sparesWidget = SparesWidget(self.sparesInfo, self.mainWindow)
        self.connectActions(self.mainWindow)
        self.mainWindow.show()
    def getDevicesInfo(self):
        confDB = self.getConfDBCxOracleConnection()
        equipDB = self.getEquipDBCxOracleConnection()
        equipDBSystem = equipDB.getSystem()
        changedSystem = confDB.getChangedSystems(equipDBSystem)
        devicesInfo = DevicesInfo()
        devicesInfo.setNewDevicesWithDHCPData(len(changedSystem[0].getAllDevices()))
        devicesInfo.setNewDevicesWithoutDHCPData(len(changedSystem[1].getAllDevices()))
        devicesInfo.setChangedDevices(len(changedSystem[2].getAllDevices())+len(changedSystem[3].getAllDevices()))
        devicesInfo.setDevicesUpToDate(len(changedSystem[4].getAllDevices()))
        return devicesInfo
    def getSparesInfo(self):
        confDB = self.getConfDBCxOracleConnection()
        equipDB = self.getEquipDBCxOracleConnection()
        sparesInfo = SparesInfo()
        sparesInfo.SparesInEquipDB = self.spareDB.getAllSparesEquipDBCount()
        sparesInfo.setSparesInConfDB(self.spareDB.getAllSparesConfDBCount())
        sparesInfo.setChangedSpares(len(self.spareDB.getChangedSpares()))
        sparesInfo.setNewSpares(len(self.spareDB.getNewSpares()))
        return sparesInfo
    def close(self):
        print "MainWindowController.close() start"
        Controller.disconnectConfDBPython(self)
        print "MainWindowController.close() end"
    def connectActions(self, mainWindow):
        mainWindow.connect(mainWindow.aboutAction, QtCore.SIGNAL("triggered()"), mainWindow.about)
        mainWindow.connect(mainWindow.refreshAction, QtCore.SIGNAL("triggered()"), self.onRefresh)
        mainWindow.connect(mainWindow.exitAction, QtCore.SIGNAL("triggered()"), self.onExit)
        mainWindow.connect(mainWindow, QtCore.SIGNAL('closeEmitApp()'), QtCore.SLOT('close()') )
        mainWindow.connect(mainWindow, QtCore.SIGNAL("mainWindowCloses"), self.close)
        ##########################################################################################
        mainWindow.connect(mainWindow.createSummaryLogAction, QtCore.SIGNAL("triggered()"), self.onCreateSummaryLog)
        mainWindow.connect(mainWindow.showLastSessionLogAction, QtCore.SIGNAL("triggered()"), self.onShowLastSessionLog)
        mainWindow.connect(mainWindow.showLastSummaryLogAction, QtCore.SIGNAL("triggered()"), self.onShowLastSummaryLog)
        ##########################################################################################
        mainWindow.connect(mainWindow.insertAction, QtCore.SIGNAL("triggered()"), self.onInsert)
        mainWindow.connect(mainWindow.updateAction, QtCore.SIGNAL("triggered()"), self.onUpdate)
        mainWindow.connect(mainWindow.deleteAction, QtCore.SIGNAL("triggered()"), self.onDelete)
        mainWindow.connect(mainWindow.tfcmunin01Action, QtCore.SIGNAL("triggered()"), self.onTfcmunin01)
        mainWindow.connect(mainWindow.swapAction, QtCore.SIGNAL("triggered()"), self.onSwap)
        ##########################################################################################
        mainWindow.connect(mainWindow.insertNewSparesAction, QtCore.SIGNAL("triggered()"), self.onInsertNewSpares)
        mainWindow.connect(mainWindow.updateSparesAction, QtCore.SIGNAL("triggered()"), self.onUpdateSpares)
        mainWindow.connect(mainWindow.deleteSparesAction, QtCore.SIGNAL("triggered()"), self.onDeleteSpares)
    def onExit(self):
        print "MainWindowController.onExit() start"
        self.mainWindow.emit(QtCore.SIGNAL('closeEmitApp()'))
        print "MainWindowController.onExit() end"
    def onCreateSummaryLog(self):
        print "MainWindowController.onCreateSummaryLog() start"
        print "MainWindowController.onCreateSummaryLog() end"
    def onShowLastSessionLog(self):
        print "MainWindowController.onShowLastSessionLog() start"
        print "MainWindowController.onShowLastSessionLog() end"
    def onShowLastSummaryLog(self):
        print "MainWindowController.onShowLastSummaryLog() start"
        print "MainWindowController.onShowLastSummaryLog() end"
    def onInsert(self):
        print "MainWindowController.onInsert() start"
        confDB = self.getConfDBCxOracleConnection()
        equipDB = self.getEquipDBCxOracleConnection()
        self.newDevicesWindowController = NewDevicesWindowController(confDB, equipDB, self)
        print "MainWindowController.onInsert() end"
    def onRefresh(self):
        print "MainWindowController.onRefresh() start"
        self.refreshWorker = RefreshWorker(self.getEquipDBCxOracleConnection(), self.getConfDBCxOracleConnection(), self.spareDB, self.devicesInfo, self.sparesInfo, self)
        self.refreshWorker.start()
        print "MainWindowController.onRefresh() end"
    def onRefreshDevicesWidget(self):
        print "MainWindowController.onRefreshDevicesWidget() start"
        self.devicesWidget.setDevicesInfo(self.devicesInfo)
        print "MainWindowController.onRefreshDevicesWidget() end"
    def onRefreshSparesWidget(self):
        print "MainWindowController.onRefreshSparesWidget() start"
        self.sparesWidget.setSparesInfo(self.sparesInfo)
        print "MainWindowController.onRefreshSparesWidget() end"
    def onUpdate(self):
        print "MainWindowController.onUpdate() start"
        print "MainWindowController.onUpdate() end"
    def onDelete(self):
        print "MainWindowController.onDelete() start"
        self.confDB = self.getConfDBCxOracleConnection()
        self.equipDB = self.getEquipDBCxOracleConnection()
        self.connectConfDBPython()
        self.db = self.getCn()
        print str(self.db)
        self.deletedevicescontroller = DeleteDevicesController(self.confDB, self.equipDB, self.db, self)
        print "MainWindowController.onDelete() end"
    def onTfcmunin01(self):
        print "MainWindowController.onTfcmunin01() start"
        print "MainWindowController.onTfcmunin01() end"
    def onSwap(self):
        print "MainWindowController.onSwap() start"
        print "MainWindowController.onSwap() end"
    def onInsertNewSpares(self):
        print "MainWindowController.onInsertNewSpares() start"
        print "MainWindowController.onInsertNewSpares() end"
    def onUpdateSpares(self):
        print "MainWindowController.onUpdateSpares() start"
        print "MainWindowController.onUpdateSpares() end"
    def onDeleteSpares(self):
        print "MainWindowController.onDeleteSpares() start"
        print "MainWindowController.onDeleteSpares() end"

