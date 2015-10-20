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
matplotlib.rcParams.update({'axes.titlesize': 'medium'})


class lPlottable():
    def __init__(self, axes, fig, params, lPlot):
        self.axes = axes
        self.fig = fig
        self.cbar_set = False
        self.params = params
        self.plot = lPlot

    
    def on_draw(self, tabOpsState):
        if tabOpsState.displayRefs: nominal, reference = lInterfaces.runview_plot(tabOpsState.runNum, self.params['name'], tabOpsState.moduleID, tabOpsState.run_data_dir, refRun = tabOpsState.refRunNum, getRef = True)
        else:
            nominal = lInterfaces.runview_plot(tabOpsState.runNum, self.params['name'], tabOpsState.moduleID, tabOpsState.run_data_dir)
            reference = None
        if 'binning' in nominal['data']['data']:
            if not tabOpsState.displayRefs: self.runview_1d_runviewReturn(nominal)
            elif tabOpsState.overLayRef:
                self.runview_1d_runviewReturn(nominal)
                if reference != None: self.runview_1d_runviewReturn(reference, True)
            elif tabOpsState.refDiff: self.runview_1d_dataMinusRef(nominal, reference)
            elif tabOpsState.refRatio: self.runview_1d_dataMinusRef(nominal, reference, True)
            
            
        elif 'xbinning' in nominal['data']['data']:  
            if tabOpsState.refDiff and tabOpsState.displayRefs: self.runview_2d_dataMinusRef(nominal, reference)
            elif tabOpsState.refRatio and tabOpsState.displayRefs: self.runview_2d_dataMinusRef(nominal, reference, True)
            else: self.runview_2d(nominal)
        
        # Titles.
        xlabel = 'Default x label'
        ylabel = 'Default y label'
        
        if 'axis_titles' in nominal:
            xlabel = nominal['axis_titles'][0]
            ylabel = nominal['axis_titles'][1]
        
        self.plot.axes.set_xlabel(xlabel)
        self.plot.axes.set_ylabel(ylabel)
        
        
    def runview_1d_dataMinusRef(self, nominal, reference, infactRatio = False):
        binwidth = nominal['data']['data']['binning'][0][1] - nominal['data']['data']['binning'][0][0]
        xs = np.linspace(nominal['data']['data']['binning'][0][0] + 0.5*binwidth,
                         nominal['data']['data']['binning'][-1][0] + 0.5*binwidth,
                         len(nominal['data']['data']['binning']))
        
        style = 1
        ys = []
        for i in range(len(nominal['data']['data']['values'])):
            if infactRatio:
                if reference['data']['data']['values'][i] == 0.0: ys.append(0)
                else: ys.append(nominal['data']['data']['values'][i]/(1.*reference['data']['data']['values'][i]))
            else:
                ys.append(nominal['data']['data']['values'][i] - reference['data']['data']['values'][i])
        
        col = 'r'
        self.add_1d_plot(xs, ys, style, col)
        minX = xs[0]
        maxX = xs[-1]
        if len(self.plot.xLims) == 0: 
            self.plot.xLims.append(minX)
            self.plot.xLims.append(maxX)
        elif minX < self.plot.xLims[0]: self.plot.xLims[0] = minX
        elif maxX > self.plot.xLims[1]: self.plot.xLims[1] = maxX 
        
        
             
    def runview_1d_runviewReturn(self, nominal, isRef = False):
        binwidth = nominal['data']['data']['binning'][0][1] - nominal['data']['data']['binning'][0][0]
        xs = np.linspace(nominal['data']['data']['binning'][0][0] + 0.5*binwidth,
                         nominal['data']['data']['binning'][-1][0] + 0.5*binwidth,
                         len(nominal['data']['data']['binning']))
        
        style = 1
        if 'style' in self.params: style = self.params['style']
        elif 'options' in self.params: 
            if 'asPoints' in self.params['options']: 
                style = 3

        col = 'b'
        if isRef: 
            style = 3
            col = 'r'
        else: 
            if 'color' in self.params: print 'hi dan', self.params
            if 'color' in self.params: 
                if self.params['color'] == 'r':
                    print 'Red is reserved for references - defaulting to blue'
                else: col = self.params['color']
                
        ys = nominal['data']['data']['values']
        print 'Before:', col
        self.add_1d_plot(xs, ys, style, col)
        minX = xs[0]
        maxX = xs[-1]
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
    
    
    def runview_2d_dataMinusRef(self, nominal, reference, infactRatio = False):
        minX = nominal['data']['data']['xbinning'][0][0]
        maxX = nominal['data']['data']['xbinning'][-1][1]
        minY = nominal['data']['data']['ybinning'][0][0]
        maxY = nominal['data']['data']['ybinning'][-1][1]
        
        pyBins = []
        for i in range(len(nominal['data']['data']['values'])):
            bins = []
            for j in range(len(nominal['data']['data']['values'][0])):
                if infactRatio:
                    if reference['data']['data']['values'][i][j] == 0.0: z = 0
                    else: z = nominal['data']['data']['values'][i][j]/(1.*reference['data']['data']['values'][i][j])
                else:
                    z = nominal['data']['data']['values'][i][j] - reference['data']['data']['values'][i][j]
                bins.append(z)
            pyBins.append(bins)
                
        npBins = np.empty([len(pyBins[0]), len(pyBins)])
        for i in range(len(pyBins)):
            for j in range(len(pyBins[0])):
                npBins[len(pyBins[0])-j-1][i] = pyBins[i][j]
        
        npBins[npBins == 0.0] = np.nan
        self.cax = self.axes.imshow(npBins, 
            interpolation='none', cmap=newMaps.viridis, 
            extent = [minX, maxX, minY, maxY],
            aspect='auto'
            )

        if not self.cbar_set:
            self.cbar = self.fig.colorbar(self.cax)
            self.cbar_set = True
            
        else:
            self.cbar.on_mappable_changed(self.cax)
            
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
        
        
    def runview_2d(self, nominal):
        minX = nominal['data']['data']['xbinning'][0][0]
        maxX = nominal['data']['data']['xbinning'][-1][1]
        minY = nominal['data']['data']['ybinning'][0][0]
        maxY = nominal['data']['data']['ybinning'][-1][1]
        
        
        pyBins = nominal['data']['data']['values']
        npBins = np.empty([len(pyBins[0]), len(pyBins)])
        for i in range(len(pyBins)):
            for j in range(len(pyBins[0])):
                npBins[len(pyBins[0])-j-1][i] = pyBins[i][j]
        
        npBins[npBins == 0.0] = np.nan
        self.cax = self.axes.imshow(npBins, 
            interpolation='none', cmap=newMaps.viridis, 
            extent = [minX, maxX, minY, maxY],
            aspect='auto'
            )

        if not self.cbar_set:
            self.cbar = self.fig.colorbar(self.cax)
            self.cbar_set = True
            
        else:
            self.cbar.on_mappable_changed(self.cax)
            
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

        if 'style' in self.params: style = self.params['style']
        elif 'options' in self.params: 
            if 'asPoints' in self.params['options']: 
                style = 3
        else: style = 0
        self.add_1d_plot(xs, ys, style)


    def add_1d_plot(self, xs, ys, style, col = 'b'):
        if style == 0:
            # Defaut - blue bars with black line on top.
            self.axes.bar(xs, ys, width = xs[1] - xs[0], alpha = 0.6, color = col, 
                linewidth = 0, align = 'center')
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.2, c = col)

        elif style == 1:
            # Just black line step on time (no fill)
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.5, c = col)

        elif style == 2:
            self.axes.plot(xs, ys, '-o', marker = '^', markerfacecolor = col, markeredgecolor='none', markersize = 8)

        elif style == 3:
            self.axes.plot(xs, ys, 'o', marker = '^', markerfacecolor = col, markeredgecolor='none', markersize = 8)
            


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

        print self.cbar_set
        if self.cbar_set:
            print 'hey'
            self.plot.fig.delaxes(self.figure.axes[1])
            self.plot.fig.subplots_adjust(right=0.90)
        
        self.cbar = self.fig.colorbar(self.cax)
        self.cbar_set = True



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
        
#         tabOpsState.outline()
        print "(Re)Plotting:", self.params['title']
        self.axes.clear()                
        for p in self.plottables: p.on_draw(tabOpsState)
        self.axes.grid()
        self.axes.set_title(self.params['title'])

        if 'xrange' in self.params: self.axes.set_xlim(self.params['xrange'])
        else: self.axes.set_xlim(self.xLims)
        if tabOpsState.refDiff == False and tabOpsState.refRatio == False:
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
        vbox = QGridLayout()
        vbox.addWidget(self.canvas, 0, 0)
        menu = QWidget(self)
        hbox = QHBoxLayout()
        menu.setLayout(hbox)
        hbox.addWidget(self.mpl_toolbar)
        vbox.addWidget(menu, 1, 0)
        hbox.setContentsMargins(0, 0, 0, 0)
        vbox.setContentsMargins(0, 0, 0, 0)
        vbox.setSpacing(0)
        self.setLayout(vbox)
        vbox.setRowStretch(0, 10)

        self.tipBut = QPushButton("?")
        self.tipBut.clicked.connect(self.show_tip)

        hbox.addWidget(self.tipBut)
