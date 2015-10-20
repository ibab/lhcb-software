import ROOT
import numpy as np
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import json

import matplotlib
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
import colormaps as newMaps

import lInterfaces


class lPlottable():
    def __init__(self, axes, fig, params, lPlot):
        self.axes = axes
        self.fig = fig
        self.cbar_set = False
        self.params = params
        self.plot = lPlot


    def on_draw(self, tabOpsState):
        plot_data = lInterfaces.runview_plot(tabOpsState.runNum, self.params['name'], 
                                             tabOpsState.moduleID, True, tabOpsState.run_data_dir)
        plot_data_x = json.loads(plot_data[0])
        if 'binning' in plot_data_x['data']['data']:
             self.json_1d(plot_data_x)
             
    def json_1d(self, plot_data):
          xs = []
          ys = []
          for bin_edges in plot_data['data']['data']['binning']:
               xs.append(0.5*(bin_edges[1] + bin_edges[0]))
          
          for value in plot_data['data']['data']['values']:
               ys.append(value)
          
          if not 'style' in self.params: style = 0
          else: style = self.params['style']    
          self.add_1d_plot(xs, ys, style)
          
          minX = min(xs)
          maxX = max(xs)
          minY = min(ys)
          maxY = max(ys)
          
          if len(self.plot.xLims) == 0: 
               self.plot.xLims.append(minX)
               self.plot.xLims.append(maxX)
          elif minX < self.plot.xLims[0]: self.plot.xLims[0] = minX
          elif maxX > self.plot.xLims[1]: self.plot.xLims[1] = maxX
               
          if len(self.plot.yLims) == 0: 
               self.plot.yLims.append(minY)
               self.plot.yLims.append(maxY)
          elif minY < self.plot.yLims[0]: self.plot.yLims[0] = minY
          elif maxY > self.plot.yLims[1]: self.plot.yLims[1] = maxY


    def plotTH1(self, g):
        xs = []
        ys = []
        for i in range(1, g.GetXaxis().GetNbins() + 1):
            xs.append(g.GetXaxis().GetBinCenter(i))
            ys.append(g.GetBinContent(i))

        if not 'style' in self.params: style = 0
        else: style = self.params['style']
        self.add_1d_plot(xs, ys, style)


    def add_1d_plot(self, xs, ys, style):
        if style == 0:
            # Defaut - blue bars with black line on top.
            self.axes.bar(xs, ys, width = xs[1] - xs[0], alpha = 0.6, color = 'b', 
                linewidth = 0, align = 'center')
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.2, c = 'k')

        elif style == 1:
            # Just black line step on time (no fill)
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.5, c = 'k')

        elif style == 2:
            if 'color' in self.params: col = self.params['color']
            else: col = 'b'
            self.axes.plot(xs, ys, '-o', marker = '^', c = col, markeredgecolor='none')

        elif style == 3:
            if 'color' in self.params: col = self.params['color']
            else: col = 'b'
            self.axes.plot(xs, ys, 'o', marker = '^', c = col, markeredgecolor='none')


    def plotTH2(self, g):
        zs = []
        pyBins = []
        for i in range(1, g.GetXaxis().GetNbins() + 1):
            bins = []
            for j in range(1, g.GetYaxis().GetNbins() + 1):
                bins.append(g.GetBinContent(i, j))
            pyBins.append(bins)

        minBin = pyBins[0][0]
        maxBin = pyBins[0][0]
        npBins = np.empty([len(pyBins[0]), len(pyBins)])
        for i in range(len(pyBins)):
            for j in range(len(pyBins[0])):
                npBins[len(pyBins[0])-j-1][i] = pyBins[i][j]
                if pyBins[i][j] > maxBin: maxBin = pyBins[i][j]
                elif pyBins[i][j] < minBin: minBin = pyBins[i][j]


        xlow = g.GetXaxis().GetBinLowEdge(1)
        xup = g.GetXaxis().GetBinUpEdge(g.GetXaxis().GetNbins())
        ylow = g.GetYaxis().GetBinLowEdge(1)
        yup = g.GetYaxis().GetBinUpEdge(g.GetYaxis().GetNbins())
        npBins[npBins == 0.0] = np.nan
        self.cax = self.axes.imshow(npBins, 
            interpolation='none', cmap=newMaps.viridis, 
            extent = [xlow, xup, ylow, yup],
            aspect='auto'
            )

        if self.cbar_set == False:
            self.cbar = self.fig.colorbar(self.cax)
            self.cbar.ax.set_aspect(11)
            self.cbar_set = True

        # else:
        #     self.cbar.on_mappable_changed(self.cax)



class mplWidget(QWidget):
    def __init__(self, params, parent=None):
        QWidget.__init__(self, parent)
        self.params = params
        self.create_main_frame()
        self.plottables = []
        self.setup_plottables()
        self.xLims = []
        self.yLims = []
        self.zLims = []
        # self.on_draw() # Drawn on call. Consider threading.


    def setup_plottables(self):
        if 'plottables' in self.params:
            for param in self.params['plottables']:
                p = lPlottable(self.axes, self.fig, param, self)
                self.plottables.append(p)

        elif 'name' in self.params:
            p = lPlottable(self.axes, self.fig, self.params, self)
            self.plottables.append(p)


    def on_draw(self, tabOpsState):
        # Redraws the figure
        del self.xLims[:]
        del self.yLims[:]
        del self.zLims[:]
        
        tabOpsState.outline()
        print "(Re)Plotting:", self.params['title']
        self.axes.clear()                
        for p in self.plottables: p.on_draw(tabOpsState)
        self.axes.grid()
        self.axes.set_title(self.params['title'])

        if 'xrange' in self.params: self.axes.set_xlim(self.params['xrange'])
        else: self.axes.set_xlim(self.xLims)
        if 'yrange' in self.params: self.axes.set_ylim(self.params['yrange'])
        else: 
             shift = 0.07*(self.yLims[1] - self.yLims[0])
             self.axes.set_ylim([self.yLims[0] - shift, self.yLims[1] + shift])
        if 'zrange' in self.params: self.plottables[0].cax.set_clim(vmin = self.params['zrange'][0], vmax = self.params['zrange'][1])
        self.fig.tight_layout()
        self.canvas.draw()
        
    
    def show_tip(self):
        if 'tip' in self.params: tip = self.params['tip']
        else: tip = 'No tip supplied.'
        box = QMessageBox()
        box.setText(tip)
        box.exec_()


    def create_main_frame(self):
        self.fig = Figure()
        
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self)
        self.axes = self.fig.add_subplot(111)

        # Create the navigation toolbar, tied to the canvas
        self.mpl_toolbar = NavigationToolbar(self.canvas, self)
        vbox = QVBoxLayout()
        vbox.addWidget(self.canvas)
        menu = QWidget(self)
        hbox = QHBoxLayout()
        menu.setLayout(hbox)
        hbox.addWidget(self.mpl_toolbar)
        vbox.addWidget(menu)
        hbox.setContentsMargins(0, 0, 0, 0)
        vbox.setContentsMargins(0, 0, 0, 0)
        vbox.setSpacing(0)
        self.setLayout(vbox)

        self.tipBut = QPushButton("?")
        self.tipBut.clicked.connect(self.show_tip)

        hbox.addWidget(self.tipBut)
