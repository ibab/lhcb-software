from mplWidget import *
from lFuncs import *

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


    def replot(self, tabOpsState):
        for plot in self.plots: plot.on_draw(tabOpsState)
        if len(self.subpages) > 0:
            self.subpages[self.tabs.currentIndex()].replot(tabOpsState)


    def setup_plots(self):
        for plot in self.params['plots']: self.plots.append(mplWidget(plot))
        for i in range(len(self.plots)):
            if 'layout' in self.params: nRows = self.params['layout'][1]
            else: nRows = 2
            row = i%nRows
            col = i/nRows
                
            self.grid_layout.addWidget(self.plots[i], row, col)


    def setup_subpages(self):
        self.tabs = QTabWidget(self)
        self.grid_layout.addWidget(self.tabs, 0, 0)
        for page in self.params['subpages']:
            subpage = lTab(page, self)
            self.subpages.append(subpage)
            self.tabs.addTab(subpage, page['title']) 
        self.tabs.currentChanged.connect(self.parentWidget().tab_changed)
