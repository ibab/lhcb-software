print 'Starting the Lovell GUI (~10secs)...' 

import sys
from lTab import *
from lTabOptions import *
from lFuncs import *
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import lInterfaces


class lovellGui(QMainWindow):
    def __init__(self, run_data_dir, parent=None):
        QMainWindow.__init__(self, parent)
        self.top_tab = QTabWidget(self)
        self.setCentralWidget(self.top_tab)
        self.top_tab.addTab(run_view(run_data_dir, self), 'Run View')
        #self.top_tab.addTab(QWidget(self), 'IV')
        
                
class run_view(QWidget):
    def __init__(self, run_data_dir, parent=None):
        QTabWidget.__init__(self, parent)
        self.run_data_dir = run_data_dir
        self.grid_layout = QGridLayout()
        setPadding(self.grid_layout)
        self.setLayout(self.grid_layout)
        self.pages = []
        self.setup_tabs()


    def setup_tabs(self):
        self.top_tab = QTabWidget(self)
        runview_config = lInterfaces.runview_config()
        
        # Do the sensor overview adjustments here.
        self.prepSensorOverview(runview_config)
        
        for key, val in runview_config.iteritems():
            page = lTab(val, self)
            self.top_tab.addTab(page, val['title'])
            self.pages.append(page)
            
        self.grid_layout.addWidget(self.top_tab, 0, 1)
        self.top_tab.currentChanged.connect(self.tab_changed)
        self.tab_options = lTabOptions(self, self.run_data_dir)
        self.tab_options.state_change.connect(self.tab_changed)
        self.grid_layout.addWidget(self.tab_options, 0, 0)
        msg = "Current run number: " + self.tab_options.state().runNum
        self.tab_options.notify(msg)
        self.tab_changed()

 
    def tab_changed(self):
        iPage = self.top_tab.currentIndex()
        self.pages[iPage].replot(self.tab_options.state(), self.tab_options)
        
        
    def prepSensorOverview(self, config):
        # Find the entry in the dictionary for the sensor overview.
        # Just dictionary gymnastics.
        
        for key, val in config.iteritems():
            if key == 'sensor_overview':
                plots = []
                # Now loop over all plots in the other tabs (inc. subtabs).
                for key2, val2 in config.iteritems():
                    if key2 == 'sensor_overview': continue      
                    if 'plots' in val2: plots += self.findSensorViewPlots(val2['plots'])
                    elif 'subpages' in val2:
                        for page in val2['subpages']: 
                            if 'plots' in page:
                                plots += self.findSensorViewPlots(page['plots'])


                plotsPerPage=[plots[x:x+4] for x in xrange(0, len(plots), 4)] 
                subpages = []
            
                for i in range(len(plotsPerPage)):
                    subpages.append({'title': str(i), 'plots': plotsPerPage[i]})
                
                if 'subpages' in val: val['subpages'] += subpages
                else: val['subpages'] = subpages
                
            
        
        
    def findSensorViewPlots(self, plot_list):
        plots = []
        for plot in plot_list:
            if 'showInSensorOverview' in plot and plot['showInSensorOverview']:
                plots.append(plot)
              
        return plots


def main():
    run_data_dir = "/afs/cern.ch/work/a/apearce/public/VetraOutput"
    if len(sys.argv) > 1:
        if sys.argv[1][0:15] == '--run-data-dir=': run_data_dir = sys.argv[1][15:]
    print 'run-data-dir set to:', run_data_dir
    app = QApplication(sys.argv) 
    form = lovellGui(run_data_dir)
    form.resize(1200, 700)
    form.show()
#     app.setStyle("plastique")
    app.exec_()


if __name__ == "__main__":
    main()