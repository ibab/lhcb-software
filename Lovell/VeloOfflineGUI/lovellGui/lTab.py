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


    def dataIvDate(self):
        return self.dataIvBox.currentText()
    
    
    def refIvDate(self):
        return self.refIvBox.currentText()


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
