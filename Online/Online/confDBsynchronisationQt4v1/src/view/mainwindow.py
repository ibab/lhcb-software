from PyQt4 import QtGui, QtCore

class MainWindow(QtGui.QMainWindow):
    def __init__(self, controller, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.controller = controller
        self.createActions()
        self.createMenu()
        self.statusBar()
        self.createToolBars()
        self.setWindowTitle("confDBsynchronisationQt4v1")
        self.setGeometry(1, 1, 800, 600) #startX, startY, length, height
        self.center()
    def center(self):
        screen = QtGui.QDesktopWidget().screenGeometry()
        size =  self.geometry()
        self.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)
    def closeEvent(self, event):
        print "MainWindow.closeEvent() start"
        self.emit(QtCore.SIGNAL("mainWindowCloses"), 1)
        event.accept()
        print "MainWindow.closeEvent() end"
    def createMenu(self):
        self.fileMenu = self.menuBar().addMenu(self.tr("&File"))
        self.fileMenu.addAction(self.aboutAction)
        self.fileMenu.addAction(self.refreshAction)
        self.fileMenu.addAction(self.exitAction)
        ###############################################################################
        ###############################################################################
        self.logMenu = self.menuBar().addMenu(self.tr("&Log"))
        self.logMenu.addAction(self.createSummaryLogAction)
        self.logMenu.addAction(self.selectLogAction)
        self.logMenu.addAction(self.showLastSessionLogAction)
        self.logMenu.addAction(self.showLastSummaryLogAction)
        ###############################################################################
        ###############################################################################
        self.configurationdbMenu = self.menuBar().addMenu(self.tr("&ConfigurationDB"))
        self.configurationdbMenu.addAction(self.insertAction)
        self.configurationdbMenu.addAction(self.updateAction)
        self.configurationdbMenu.addAction(self.deleteAction)
        self.configurationdbMenu.addAction(self.connectAction)
        self.configurationdbMenu.addAction(self.tfcmunin01Action)
        self.configurationdbMenu.addAction(self.swapAction)
        ##########################################################
        self.configurationdbMenu.addAction(self.insertNewSparesAction)
        self.configurationdbMenu.addAction(self.updateSparesAction)
        self.configurationdbMenu.addAction(self.deleteSparesAction)
    def createActions(self):
        self.exitAction = QtGui.QAction(QtGui.QIcon("images/quit.png"), self.tr("&Exit"), self)
        self.exitAction.setShortcut(self.tr("Ctrl+X"))
        self.exitAction.setStatusTip(self.tr("exit"))
        #########################################################################################################
        self.refreshAction = QtGui.QAction(self.tr("&Refresh View"), self)
        self.refreshAction.setShortcut(self.tr("Ctrl+R"))
        self.refreshAction.setStatusTip(self.tr("Refreshs the window"))
        #########################################################################################################
        self.aboutAction = QtGui.QAction(self.tr("&About"), self)
        self.aboutAction.setStatusTip(self.tr("About this program"))
        #########################################################################################################
        self.insertAction = QtGui.QAction(QtGui.QIcon("images/new.png"), self.tr("&Insert new devices"), self)
        self.insertAction.setShortcut(self.tr("Ctrl+I"))
        self.insertAction.setStatusTip(self.tr("Copies devices from lhcbintegration to configuration database"))
        #########################################################################################################
        self.updateAction = QtGui.QAction(QtGui.QIcon("images/update.png"), self.tr("&Update devices in configuration database"), self)
        self.updateAction.setShortcut(self.tr("Ctrl+U"))
        self.updateAction.setStatusTip(self.tr("Updates existing devices in configuration database"))
        #########################################################################################################
        self.deleteAction = QtGui.QAction(QtGui.QIcon("images/delete.png"), self.tr("&Delete all devices from configuration database"), self)
        self.deleteAction.setShortcut(self.tr("Ctrl+D"))
        self.deleteAction.setStatusTip(self.tr("Deletes all devices from configuration database"))
        #########################################################################################################
        self.connectAction = QtGui.QAction(self.tr("&Connect devices"), self)
        self.connectAction.setShortcut(self.tr("Ctrl+T"))
        self.connectAction.setStatusTip(self.tr("Connects devices to hugins and hugin02,03,04 to hugin01"))
        #########################################################################################################
        self.tfcmunin01Action = QtGui.QAction(self.tr("&Connect hugins to TFCMUNIN01"), self)
        self.tfcmunin01Action.setShortcut(self.tr("Ctrl+T"))
        self.tfcmunin01Action.setStatusTip(self.tr("Connects the hugin01 to TFCMUNIN01"))
        #########################################################################################################
        self.swapAction = QtGui.QAction(self.tr("&Swap two devices"), self)
        self.swapAction.setShortcut(self.tr("Ctrl+S"))
        self.swapAction.setStatusTip(self.tr("Swaps tow devices, e.g. swaps a device with a spare"))
        #########################################################################################################
        self.insertNewSparesAction = QtGui.QAction(self.tr("&Insert new spares into configuation database"), self)
        self.insertNewSparesAction.setStatusTip(self.tr("Copies new spares from lhcbintegration to configuration database"))
        #########################################################################################################
        self.updateSparesAction = QtGui.QAction(self.tr("&Update spares in configuration database"), self)
        self.updateSparesAction.setStatusTip(self.tr("Updates existing spares in configuration database"))
        #########################################################################################################
        self.deleteSparesAction = QtGui.QAction(self.tr("&Delete spares in configuration database"), self)
        self.deleteSparesAction.setStatusTip(self.tr("Deletes all spares in configuration database"))
        #########################################################################################################
        self.createSummaryLogAction = QtGui.QAction(self.tr("&Create summary log"), self)
        self.createSummaryLogAction.setStatusTip(self.tr("Creates a summary log about changed data"))
        #########################################################################################################
        self.selectLogAction = QtGui.QAction(self.tr("Select logfile"), self)
        self.selectLogAction.setStatusTip(self.tr("Select a log file and view it"))
        #########################################################################################################
        self.showLastSessionLogAction = QtGui.QAction(self.tr("&Show last session log"), self)
        self.showLastSessionLogAction.setStatusTip(self.tr("Shows the last session log"))
        #########################################################################################################
        self.showLastSummaryLogAction = QtGui.QAction(self.tr("&Show last summary log"), self)
        self.showLastSummaryLogAction.setStatusTip(self.tr("Shows the last session log"))
        #########################################################################################################
    def createToolBars(self):
        self.deviceToolBar = self.addToolBar(self.tr("device"))
        self.deviceToolBar.addAction(self.exitAction)
        self.deviceToolBar.addAction(self.insertAction)
        self.deviceToolBar.addAction(self.updateAction)
        self.deviceToolBar.addAction(self.deleteAction)
    def about(self):
        print "MainWindow.about() start"
        QtGui.QMessageBox.information(self, "About", "confDBsnychronisation\n\nCopyright (c) 2008 CERN")
        print "MainWindow.about() end"