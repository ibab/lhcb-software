import numpy as np

from PyQt4.QtGui import (
    QGridLayout,
    QHBoxLayout,
    QPalette,
    QPushButton,
    QMessageBox,
    QWidget
)

import matplotlib
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
from veloview.config.run_view import DEFAULT_DRAW_OPTIONS
import colormaps as newMaps
import lInterfaces
matplotlib.rcParams.update({'axes.titlesize': 'medium'})
matplotlib.rcParams.update({'legend.fontsize': 'medium'})
matplotlib.rcParams.update({'legend.framealpha': '0.5'})


class NavigationToolbar_mod(NavigationToolbar):
    # only display the buttons we need
    toolitems = [t for t in NavigationToolbar.toolitems if
                 t[0] in ('Home', 'Pan', 'Zoom', 'Save')]


class lPlottable():
    def __init__(self, axes, fig, params, lPlot):
        self.axes = axes
        self.fig = fig
        self.cbar_set = False
        self.params = params
        self.plot = lPlot
        # Set an empty options dictionary if one isn't present
        self.options = self.params.get('options', {})

        self._initialise_default_options()

    def _initialise_default_options(self):
        """Set default values for any missing cosmetic option keys."""
        for key, default_value in DEFAULT_DRAW_OPTIONS.iteritems():
            if key not in self.options:
                self.options[key] = default_value

    def tab(self):
        return self.plot.parent()
    
    def on_draw(self, tabOpsState, notifyBox):
        print self.params['name']
        if 'normalise' in self.params and self.params['normalise']: 
            norm = True
        else: norm = False
        moduleID = tabOpsState.moduleID
        # TODO(AP) I have no idea what this means, why is the number shifted?
        moduleID += self.options['sensorShift']
        if tabOpsState.displayRefs:
            if 'isIV' in self.params and self.params['isIV']:
                self.plot.ext = ' - sensor ' + str(moduleID)
                nominal, reference = lInterfaces.IV_plot(self.params['name'], moduleID, 
                                                         self.tab().dataIvDate(),
                                                         self.tab().IV_directory,
                                                         self.tab().sensor_mapping,
                                                         refDate = self.tab().refIvDate(),
                                                         getRef = True,
                                                         notifyBox = notifyBox)
            else:
                if 'sensor_dependent' in self.plot.params and self.plot.params['sensor_dependent']:
                    self.plot.ext = ' - sensor ' + str(moduleID)
                # Coerce non-Python objects, like QString, to Python objects
                refRun = tabOpsState.refRunNum
                try:
                    refRun = int(refRun)
                except ValueError:
                    refRun = None
                nominal, reference = lInterfaces.runview_plot(int(tabOpsState.runNum), self.params['name'],
                                                              moduleID, str(tabOpsState.run_data_dir), 
                                                              refRun = refRun, getRef = True, 
                                                              notifyBox = notifyBox,
                                                              normalise = norm)
                 
        else:
            if 'isIV' in self.params and self.params['isIV']: 
                self.plot.ext = ' - sensor ' + str(moduleID)
                nominal = lInterfaces.IV_plot(self.params['name'], moduleID, 
                                              self.tab().dataIvDate(),
                                              self.tab().IV_directory,
                                              self.tab().sensor_mapping,
                                              notifyBox = notifyBox) 
            else:

                if 'sensor_dependent' in self.plot.params and self.plot.params['sensor_dependent']:
                    self.plot.ext = ' - sensor ' + str(moduleID)
                nominal = lInterfaces.runview_plot(tabOpsState.runNum, self.params['name'], 
                                                   moduleID, tabOpsState.run_data_dir, 
                                                   normalise = norm, notifyBox = notifyBox)
            reference = None
        if 'binning' in nominal['data']['data'] and len(nominal['data']['data']['binning']) > 0:
            if not tabOpsState.displayRefs: self.runview_1d_runviewReturn(nominal)
            elif tabOpsState.overLayRef:
                self.runview_1d_runviewReturn(nominal)
                if reference != None: self.runview_1d_runviewReturn(reference, True)
            elif tabOpsState.refDiff: self.runview_1d_dataMinusRef(nominal, reference)
            elif tabOpsState.refRatio: self.runview_1d_dataMinusRef(nominal, reference, True)
            
            
        elif 'xbinning' in nominal['data']['data'] and len(nominal['data']['data']['xbinning']) > 0: 
            if self.options['asText']: self.runview_2d_text(nominal)
            elif tabOpsState.refDiff and tabOpsState.displayRefs: self.runview_2d_dataMinusRef(nominal, reference)
            elif tabOpsState.refRatio and tabOpsState.displayRefs: self.runview_2d_dataMinusRef(nominal, reference, True)
            else: self.runview_2d(nominal)
        
        # Titles.
        xlabel = 'Default x label'
        ylabel = 'Default y label'
        zlabel = 'Default z label'
        
        if 'axis_titles' in self.plot.params:
            xlabel = self.plot.params['axis_titles'][0]
            ylabel = self.plot.params['axis_titles'][1]        
            if len(self.plot.params['axis_titles']) == 3: zlabel = ['axis_titles'][2]
        
        elif 'axis_titles' in nominal:
            xlabel = nominal['axis_titles'][0]
            ylabel = nominal['axis_titles'][1]        
            if len(nominal['axis_titles'][1]) == 3: zlabel = ['axis_titles'][2]
            
        
        self.plot.axes.set_xlabel(xlabel)
        self.plot.axes.set_ylabel(ylabel)
        if self.cbar_set: self.cbar.set_label(zlabel)
     
        
    def runview_2d_text(self, nominal):
        minX = nominal['data']['data']['xbinning'][0][0]
        maxX = nominal['data']['data']['xbinning'][-1][1]
        minY = nominal['data']['data']['ybinning'][0][0]
        maxY = nominal['data']['data']['ybinning'][-1][1]
        halfBinX = 0.5*(nominal['data']['data']['xbinning'][0][1] + nominal['data']['data']['xbinning'][0][0])
        halfBinY = 0.5*(nominal['data']['data']['ybinning'][0][1] + nominal['data']['data']['ybinning'][0][0])
        
        for i in range(len(nominal['data']['data']['values'])):
            for j in range(len(nominal['data']['data']['values'][0])):
                z = nominal['data']['data']['values'][i][j]
                
                if z == 0.0: continue
                x = nominal['data']['data']['xbinning'][i][1] + halfBinX + 0.6
                y = nominal['data']['data']['ybinning'][j][1] + halfBinY
                
                if int(z) % 2 == 0: y = 2.5
                else: y = -2.5
                self.axes.annotate(str(int(z)), xy=(x, y), xycoords='data', fontsize=9,
                                   horizontalalignment='center', verticalalignment='center', rotation=90, weight = 'demibold')
        
        
    def runview_1d_dataMinusRef(self, nominal, reference, infactRatio = False):
        binwidth = nominal['data']['data']['binning'][0][1] - nominal['data']['data']['binning'][0][0]
        xs = []
        for bin in nominal['data']['data']['binning']: xs.append(0.5*(bin[0] + bin[1]))
            
        ys = []
        for i in range(len(nominal['data']['data']['values'])):
            if infactRatio:
                if reference['data']['data']['values'][i] == 0.0: ys.append(0)
                else: ys.append(nominal['data']['data']['values'][i]/(1.*reference['data']['data']['values'][i]))
            else:
                ys.append(nominal['data']['data']['values'][i] - reference['data']['data']['values'][i])
        
        style = 0
        if not infactRatio: col = 'r'
        else: col = 'g'
        self.add_1d_plot(xs, ys, style, col)
        minX = min(xs)
        maxX = max(xs)
        if len(self.plot.xLims) == 0: 
            self.plot.xLims.append(minX)
            self.plot.xLims.append(maxX)
        elif minX < self.plot.xLims[0]: self.plot.xLims[0] = minX
        elif maxX > self.plot.xLims[1]: self.plot.xLims[1] = maxX 
        
        
             
    def runview_1d_runviewReturn(self, nominal, isRef = False):
        binwidth = nominal['data']['data']['binning'][0][1] - nominal['data']['data']['binning'][0][0]
        xs = []
        for bin in nominal['data']['data']['binning']: xs.append(0.5*(bin[0] + bin[1]))
        style = self.options['style']

        col = 'b'
        if isRef: 
            style = 3
            col = 'r'
        else: 
            if self.options['color'] is not None: 
                if self.options['color'] == 'r' and style == 0:
                    msg = 'Red bars are reserved for references - defaulting to blue'
                    print msg
                    self.tab().notify(msg)
                elif self.options['color'] == 'g' and style == 0:
                    msg = 'Green bars are reserved for references - defaulting to blue'
                    print msg
                    self.tab().notify(msg)
                else: col = self.options['color']
                
        ys = nominal['data']['data']['values']
        self.add_1d_plot(xs, ys, style, col)
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
        xHalfBinWidth = 0.5*(nominal['data']['data']['xbinning'][0][1] 
                         - nominal['data']['data']['xbinning'][0][0])
        yHalfBinWidth = 0.5*(nominal['data']['data']['ybinning'][0][1] 
                         - nominal['data']['data']['ybinning'][0][0])
        
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
            self.cbar = self.fig.colorbar(self.cax, aspect=12)
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

        style = self.options['style']
        if 'asPoints' in self.options:
            style = 3
        self.add_1d_plot(xs, ys, style)


    def add_1d_plot(self, xs, ys, style, col = 'b'):
        lab = 'Default'
        if 'title' in self.params:
            lab = self.params['title']
        if style == 0:
            # Defaut - blue bars with black line on top.
            # Need slightly different format to fill.
            xsMod = []
            ysMod = []
            halfBinWidth = 0.5*(xs[1] - xs[0])
            for i in range(len(xs)):
                xsMod.append(xs[i] - halfBinWidth)
                xsMod.append(xs[i] + halfBinWidth)
                ysMod.append(ys[i])
                ysMod.append(ys[i])
#                     
            self.axes.bar(xs[1], ys[1], width = 0, align = 'center', linewidth = 0, alpha = 0.5, color = col, label = lab)  
            self.axes.fill_between(xsMod, ysMod, np.zeros(len(xsMod)), alpha = 0.5, color = col, edgecolor = None)
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.0, c = 'k')
            # This bit is ugly. Since the fill_between method does not support legend, add an invisible
            # bar, just for the first bin.

        elif style == 1:
            # Just black line step on time (no fill)
            self.axes.step(xs, ys, where = 'mid', linewidth = 1.5, c = col, label = lab)

        elif style == 2:
            m = self.options['marker']
            self.axes.plot(xs, ys, '-o', marker = m, markerfacecolor = col, markeredgecolor='none', markersize = 8, label = lab)

        elif style == 3:
            m = self.options['marker']
            self.axes.plot(xs, ys, 'o', marker = m, markerfacecolor = col, markeredgecolor='none', markersize = 8, label = lab)
            
        elif style == 4:
            # Solid line.
            self.axes.plot(xs, ys, '-', color = col, linewidth = 2.5, label = lab)
            


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

        if self.cbar_set:
            self.plot.fig.delaxes(self.figure.axes[1])
            self.plot.fig.subplots_adjust(right=0.90)
        
        self.cbar = self.fig.colorbar(self.cax)
        self.cbar_set = True



class mplWidget(QWidget):
    def __init__(self, params, parent=None):
        QWidget.__init__(self, parent)
        self.ext = ""
        self.params = params
        # Set an empty options dictionary if one isn't present
        self.options = self.params.get('options', {})
        self.create_main_frame()
        self.plottables = []
        self.setup_plottables()
        self.xLims = []
        self.yLims = []
        self.zLims = []
        # self.on_draw() # Drawn on call. Consider threading.

        self._initialise_default_options()

    def _initialise_default_options(self):
        """Set default values for any missing cosmetic option keys."""
        for key, default_value in DEFAULT_DRAW_OPTIONS.iteritems():
            if key not in self.options:
                self.options[key] = default_value


    def setup_plottables(self):
        if 'plottables' in self.params:
            for param in self.params['plottables']:
                p = lPlottable(self.axes, self.fig, param, self)
                self.plottables.append(p)

        elif 'name' in self.params:
            p = lPlottable(self.axes, self.fig, self.params, self)
            self.plottables.append(p)


    def on_draw(self, tabOpsState, notifyBox):
        # Redraws the figure
        self.axes.set_title(self.params['title'])
        del self.xLims[:]
        del self.yLims[:]
        del self.zLims[:]
        
#         tabOpsState.outline()
        print "(Re)Plotting:", self.params['title']
        self.axes.clear()                
        for p in self.plottables: p.on_draw(tabOpsState, notifyBox)
        self.axes.grid()
        self.axes.set_title(self.params['title'] + self.ext)

        if 'xrange' in self.params: self.axes.set_xlim(self.params['xrange'])
        else:
            if len(self.xLims) == 2: self.axes.set_xlim(self.xLims)
        
        
        manSetY = False
        if tabOpsState.refDiff == False and tabOpsState.refRatio == False: manSetY=True
        if tabOpsState.displayRefs == False: manSetY=True
        
        if manSetY:
            if 'yrange' in self.params: 
                print 'Setting y axis range as per run_view configuration'
                self.axes.set_ylim(self.params['yrange'])
            else: 
                print 'Setting y axis range automatically'
                if len(self.yLims) == 2: 
                    print self.yLims[1], self.yLims[0]
                    shift = 0.05*(self.yLims[1] - self.yLims[0])
                    self.axes.set_ylim([self.yLims[0] - shift, self.yLims[1] + shift]) 
            if 'zrange' in self.params: self.plottables[0].cax.set_clim(vmin = self.params['zrange'][0], vmax = self.params['zrange'][1])
        else: print 'Automatically setting y axis range (using matplotlib).'
        
        self.fig.tight_layout()
        if self.options['legend']:
            if manSetY: self.axes.legend(loc = 'best')
        self.canvas.draw()
        self.fig.tight_layout()
        
    
    def show_tip(self):
        if 'tip' in self.params: tip = self.params['tip']
        else: tip = 'No tip supplied.'
        box = QMessageBox()
        box.setText(tip)
        box.exec_()


    def create_main_frame(self):
#         p = self.palette()
#         p.setColor(self.backgroundRole(), Qt.white)
#         self.setPalette(p)
        figColor = self.palette().color(QPalette.Background)
        self.fig = Figure(facecolor = [figColor.red()/255., figColor.blue()/255., figColor.green()/255.])
        
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self)
        self.axes = self.fig.add_subplot(111)

        # Create the navigation toolbar, tied to the canvas
        self.mpl_toolbar = NavigationToolbar_mod(self.canvas, self)
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
