import sys
from lTab import *
from lTabOptions import *
from lFuncs import *
import lInterfaces


class lovellGUI(QMainWindow):
    def __init__(self, run_data_dir, parent=None):
        QMainWindow.__init__(self, parent)
        self.run_data_dir = run_data_dir
        self.widg = QWidget(self)
        self.grid_layout = QGridLayout()
        setPadding(self.grid_layout)
        self.widg.setLayout(self.grid_layout)
        self.pages = []
        self.setup_tabs()
        self.setCentralWidget(self.widg)


    def setup_tabs(self):
        self.top_tab = QTabWidget(self)
        for key, val in lInterfaces.runview_config().iteritems():
            page = lTab(val, self)
            self.top_tab.addTab(page, val['title'])
            self.pages.append(page)
        self.grid_layout.addWidget(self.top_tab, 0, 1)
        self.top_tab.currentChanged.connect(self.tab_changed)
        self.tab_options = lTabOptions(self, self.run_data_dir)
        self.tab_options.state_change.connect(self.tab_changed)
        self.grid_layout.addWidget(self.tab_options, 0, 0)
        self.tab_changed()

 
    def tab_changed(self):
        iPage = self.top_tab.currentIndex()
        self.pages[iPage].replot(self.tab_options.state())


def main():
    print 'Starting the Lovell GUI (~10secs)...' 
    run_data_dir = "/afs/cern.ch/work/a/apearce/public/VetraOutput"
    if len(sys.argv) > 1:
        if sys.argv[1][0:15] == '--run-data-dir=': run_data_dir = sys.argv[1][15:]
    print 'run-data-dir set to:', run_data_dir
    app = QApplication(sys.argv) 
    form = lovellGUI(run_data_dir)
    form.resize(1200, 700)
    form.show()
#     app.setStyle("plastique")
    app.exec_()


if __name__ == "__main__":
    main()