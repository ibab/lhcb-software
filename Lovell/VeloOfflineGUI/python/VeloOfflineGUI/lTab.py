import glob
import os
import pickle

from PyQt4.QtCore import Qt
from PyQt4.QtGui import (
    QComboBox,
    QGridLayout,
    QGroupBox,
    QLabel,
    QTabWidget,
    QWidget,
)

from lFuncs import setPadding
from mplWidget import mplWidget
import lInterfaces
import pprint

def numeric_compare(x, y):
    print x, y
    return int(x[0:6]) - int(y[0:6])

class lTab(QWidget):
    def __init__(self, params, parent=None):
        QWidget.__init__(self, parent)
        self.params = params
        self.grid_layout = QGridLayout(self)
        self.plots = []
        self.subpages = []
        setPadding(self.grid_layout)
        if 'plots' in self.params: self.setup_plots()
        elif 'subpages' in self.params: self.setup_subpages()


    def replot(self, tabOpsState, notifyBox):
        for plot in self.plots: plot.on_draw(tabOpsState, notifyBox)
        if len(self.subpages) > 0:
            self.subpages[self.tabs.currentIndex()].replot(tabOpsState, notifyBox)

    def replotTrend(self, tabOpsState):
        for plot in self.plots: plot.on_draw_trend(tabOpsState)

    def dataIvDate(self):
        return self.dataIvBox.currentText()
    
    
    def refIvDate(self):
        return self.refIvBox.currentText()

    
    def trendVariable(self):
        index = self.variableBox.currentIndex()
        variableList = lInterfaces.trending_variables()
        variableListSorted = sorted(variableList, reverse=True)
        return variableListSorted[index][0]


    def trendVariable2d(self):
        index = self.variableBox2d.currentIndex()
        variableList = lInterfaces.trending_variables()
        variableListSorted = sorted(variableList, reverse=True)
        return variableListSorted[index][0]

    
    def is2d(self):
        return self.two_dimension


    def trendStartNum(self):
        return self.runNumStartBox.currentText()

    
    def trendEndNum(self):
        return self.runNumEndBox.currentText()


    def setup_plots(self):
        for plot in self.params['plots']: self.plots.append(mplWidget(plot))
        for i in range(len(self.plots)):
            if 'layout' in self.params: nRows = self.params['layout'][0]
            else: nRows = 2
            row = i/nRows
            col = i%nRows   
            self.grid_layout.addWidget(self.plots[i], row, col)


    def modifyPageForIV(self, IV_directory):
        # Called after plots are setup.
        self.IV_directory = IV_directory
        self.dataIvBox = QComboBox(self)
        self.dataIvBox.currentIndexChanged.connect(self.parent().tab_options.state_changed)
        self.refIvBox = QComboBox(self)
        self.refIvBox.currentIndexChanged.connect(self.parent().tab_options.state_changed)
        
        widg = QGroupBox('IV File Selection', self)
        widgLayout = QGridLayout(widg)
        lab1 = QLabel("Data file:")
        lab1.setAlignment(Qt.AlignRight)
        lab2 = QLabel("Ref file:")
        lab2.setAlignment(Qt.AlignRight)
        widgLayout.addWidget(lab1, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.dataIvBox, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(lab2, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.refIvBox, 0, widgLayout.columnCount(), 1, 1)
        self.grid_layout.addWidget(widg, self.grid_layout.rowCount(), 0, 1, self.grid_layout.columnCount())
        fileNames = self.getIVfileName(IV_directory)
        if (len(fileNames)) == 0:
            msg = 'No IV files found'
            self.parent().tab_options.notify(msg, 'No IV files found')
            print msg
            
        # Sort them.
        fileNameSorted = sorted(fileNames, reverse=True)
        for file in fileNameSorted:
            self.dataIvBox.addItem(file)
            self.refIvBox.addItem(file)
        
        fn = os.path.join(os.path.dirname(__file__), 'sensor_mapping.p')
        self.sensor_mapping = pickle.load(open(fn, "rb" ))
    

    def modifyPageForTrending(self, run_data_dir, two_dimension):
        # Used to add options bar for trending plots
        self.run_data_dir = run_data_dir 
        self.two_dimension = two_dimension
        self.variableBox = QComboBox(self)
        self.variableBox.currentIndexChanged.connect(self.parent().tab_options.state_changed)
        self.variableBox2d = QComboBox(self)
        self.variableBox2d.currentIndexChanged.connect(self.parent().tab_options.state_changed)
        # If two dimensional options not set, hide second variable box
        if two_dimension == False:
            self.variableBox2d.setVisible(False)
        self.runNumStartBox = QComboBox(self)
        self.runNumStartBox.currentIndexChanged.connect(self.parent().tab_options.state_changed)
        self.runNumEndBox = QComboBox(self)
        self.runNumEndBox.currentIndexChanged.connect(self.parent().tab_options.state_changed)

        widg = QGroupBox('Trending options', self)
        widgLayout = QGridLayout(widg)
        if two_dimension == True:
            lab1 = QLabel("X trend variable:")
        else:
            lab1 = QLabel("Trend variable:")
        lab1.setAlignment(Qt.AlignRight)
        lab2 = QLabel("Y trend variable:")
        lab2.setAlignment(Qt.AlignRight)
        # If two dimensional options not set, hide second variable label
        if two_dimension == False:
            lab2 = QLabel("")
        lab3 = QLabel("Initial run:")
        lab3.setAlignment(Qt.AlignRight)
        lab4 = QLabel("Final run:")
        lab4.setAlignment(Qt.AlignRight)
        widgLayout.addWidget(lab1, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.variableBox, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(lab2, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.variableBox2d, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(lab3, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.runNumStartBox, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(lab4, 0, widgLayout.columnCount(), 1, 1)
        widgLayout.addWidget(self.runNumEndBox, 0, widgLayout.columnCount(), 1, 1)
        self.grid_layout.addWidget(widg, self.grid_layout.rowCount(), 0, 1, self.grid_layout.columnCount())

        # Retrieve list of trending variables
        variableList = lInterfaces.trending_variables()
        variableListSorted = sorted(variableList, reverse=True)
        
        for i in xrange(len(variableList)):
                self.variableBox.addItem(variableListSorted[i][1])
                self.variableBox2d.addItem(variableListSorted[i][1])
        
        runList = lInterfaces.run_list(self.run_data_dir)
        runListSorted = sorted(runList)

        for run in runListSorted:
                self.runNumStartBox.addItem(str(run))
                self.runNumEndBox.addItem(str(run))
        listEntries = len(runListSorted)
        self.runNumEndBox.setCurrentIndex(listEntries-1)

    def getIVfileName(self, direc):
        files = []
        st = direc + "/*.iv"
        "Searching for IV files in :", st
        for file in glob.glob(st): files.append(file.split('/')[-1])
        return files
    
    
    def setup_subpages(self):
        self.tabs = QTabWidget(self)
        self.grid_layout.addWidget(self.tabs, 0, 0)
        for page in self.params['subpages']:
            subpage = lTab(page, self)
            self.subpages.append(subpage)
            self.tabs.addTab(subpage, page['title']) 
        self.tabs.currentChanged.connect(self.parentWidget().tab_changed)

