'''
Module that defines class MultiPlot, wrapper to THStack and TMultiGraph
'''

import ROOT as r

def setPlotAttributes(histo, color = None, markerStyle = None, fill=False, markerSize=None, lineStyle=None):
    if markerStyle:
        histo.SetMarkerStyle(markerStyle)
    if color:
        histo.SetMarkerColor(color)
        histo.SetLineColor(color)
        if fill: histo.SetFillColor(color)
    if markerSize:
        histo.SetMarkerSize(markerSize)
    if lineStyle:
        histo.SetLineStyle(lineStyle)
    
    
class MultiPlot:
    """
    Class wrapper to THStack and TMultiGraph which also fill legend
    kind can be h (histo), g (graph) or s (stack)
    if autoStyle == True the various histos will have different colors ad markerstyles

    histos can be provided at construction or via the Add method:
    Costructor take dictionaries with histos, legends, styles, colors, markerStyles
    
    histos: dictionary with histos to be added
    
    legends: dictionary with legends, if a key is present in the histos dict but not in the legends dict than it won't be added to the legend
    
    styles, colors, markerStyles, fillHistos dictionary for style, color markerStyle and fill,
    if a key is present in the histos dict but not in the styles,... one will keep the default (automatic if autoStyle==Trforue or the one already of the histo)
    fill can be True of False (histo filled or not), default is True for stack and false for histos

    style is a number in [1, 8], if positive the markerStyle will be full, if negative empty
    """
    colori=[r.kBlue, r.kRed, r.kGreen+2, r.kMagenta+1, r.kOrange-3, r.kYellow, r.kCyan, r.kBlack]
    colori += colori[2:]+colori[:2]
    markersA=[20,21,22,23,29,33,34,8]*2
    markersC=[24,25,26,32,30,27,28,4]*2

    def __init__(self, name, title='', kind='h', histos={}, labels={}, styles = {}, colors={}, markerStyles={}, markerSizes={}, lineStyles={}, fillHisto={}, legPosition = (.80, 0.80, .99, .99), rangeY = [None, None], rangeX = [None, None], autoStyle=True, legMarker=None, drawLegend = True, hlines = [], vlines = [], hlines_colors ={},  vlines_colors ={}, hlines_styles ={},  vlines_styles ={}, hlines_width ={},  vlines_width ={}):
        self.kind = kind
        self.autoStyle = autoStyle
        self.numHistos = 0
        self.leg = r.TLegend(*legPosition)
        self.leg.SetFillColor(0)
        self.hist_list = [0]*32
        self.rangeY = rangeY[:]
        self.rangeX = rangeX[:]
        self.drawLegend = drawLegend
        self.hlines = hlines[:]
        self.vlines = vlines[:]
        self.hlines_colors = hlines_colors
        self.vlines_colors = vlines_colors
        self.hlines_styles = hlines_styles
        self.vlines_styles = vlines_styles
        self.hlines_width = hlines_width
        self.vlines_width = vlines_width

        if self.kind.lower() in ('h', 'histo', 'histos'):
            self.hs = r.THStack(name, title)
            self.legMarker = 'l'
            self.DrawOption = 'nostack'
        elif self.kind.lower() in ('s', 'stack'):
            self.hs = r.THStack(name, title)
            self.legMarker = 'f'
            self.DrawOption = ''
        elif self.kind.lower() in ('p', 'profile'):
            self.hs = r.THStack(name, title)
            self.legMarker = 'p'
            self.DrawOption = 'nostack'
        elif self.kind.lower() in ('g', 'graph', 'graphs'):
            self.hs = r.TMultiGraph(name, title)
            self.legMarker = 'p'
            self.DrawOption = 'ap'
        else:
            raise ValueError('kind of MultiPlot not recognized')

        if legMarker:
            self.legMarker = legMarker

        for key, _histo in histos.items():
            _label = labels.get(key)
            _style = styles.get(key)
            _color = colors.get(key)
            _markerStyle = markerStyles.get(key)
            _markerSize = markerSizes.get(key)
            _lineStyle = lineStyles.get(key)
            _fill = fillHisto.get(key)
            self.Add(histo=_histo,label=_label, style=_style, color=_color,markerStyle=_markerStyle, markerSize = _markerSize, lineStyle = _lineStyle, fill=_fill)
            

    def Add(self, histo, label=None, style=None, color=None, markerStyle=None, markerSize=None, lineStyle=None, fill=None):
        """
        if style is positive the marker is full, if negative it'is empty
        """
        self.hist_list[self.numHistos] = histo.Clone()
        self.numHistos += 1
        _style, _color, _markerStyle = None, None, None
        if self.autoStyle:
            _style = self.numHistos
        if style: _style = style
        if _style:
            _color = MultiPlot.colori[abs(_style)-1]
            _markerStyle = MultiPlot.markersA[_style-1] if _style>0 else MultiPlot.markersC[abs(_style)-1] 
        if color: _color = color
        if markerStyle: _markerStyle = markerStyle
        _markerSize = markerSize
        _lineStyle = lineStyle
        if fill != None: _fill = fill
        else: _fill = self.kind == 's'
        setPlotAttributes(self.hist_list[self.numHistos-1], color=_color, markerStyle=_markerStyle, markerSize=_markerSize, lineStyle = _lineStyle, fill=_fill)
        self.hs.Add(self.hist_list[self.numHistos-1])
        if label:
            self.leg.AddEntry(self.hist_list[self.numHistos-1], label, self.legMarker)


    def AddLine(self, val, kind='h', color=None, style=None, width=None):
        '''
        kind can be "v" "vertical" or "h" "horizontal"
        '''
        if kind.lower() in ['v', 'vertical']:
            self.vlines.append(val)
            if color != None: self.vlines_colors[val] = color
            if style != None: self.vlines_styles[val] = style
            if width != None: self.vlines_width[val] = width
        elif kind.lower() in ['h', 'horizontal']:
            self.hlines.append(val)
            if color != None: self.hlines_colors[val] = color
            if style != None: self.hlines_styles[val] = style
            if width != None: self.hlines_width[val] = width

    
    def Draw(self):
        if self.kind.lower() in ('p', 'profile'):
            newMin, newMax = None, None
            if self.rangeY[0] == None:
                newMin = min([min([h.GetBinContent(iBin) for iBin in range(h.GetNbinsX()) if h.GetBinContent(iBin) != 0]) for h in self.hist_list if h])
            if self.rangeY[1] == None:
                newMax = max([h.GetMaximum() for h in self.hist_list if h])
            margin = ((self.rangeY[1] or newMax) - (self.rangeY[0] or newMin))/10.
            if newMin != None: self.rangeY[0] = newMin - margin
            if newMax != None: self.rangeY[1] = newMax + margin
        if self.rangeY[0] != None: self.hs.SetMinimum(self.rangeY[0])
        if self.rangeY[1] != None: self.hs.SetMaximum(self.rangeY[1])
        self.hs.Draw(self.DrawOption)
        if self.rangeX != [None, None]:
            # minX, maxX = (self.hist_list[0].GetBinLowEdge(1), self.hist_list[0].GetBinLowEdge(self.hist_list[0].GetNbinsX())+self.hist_list[0].GetBinWidth(self.hist_list[0].GetNbinsX()))
            self.hs.GetXaxis()
            minX, maxX = (self.hs.GetXaxis().GetXmin(), self.hs.GetXaxis().GetXmax())
            if self.rangeX[0] != None and self.rangeX[0] > minX: minX = self.rangeX[0] 
            if self.rangeX[1] !=  None and self.rangeX[1] < maxX: maxX = self.rangeX[1]
            self.hs.GetXaxis().SetRangeUser(minX,maxX)
            self.hs.Draw(self.DrawOption)
        if self.drawLegend: self.leg.Draw('same')
        if self.hlines:
            self.lines = {}
            for val in self.hlines:
                self.lines[val] = r.TLine(self.hs.GetXaxis().GetXmin(),val,self.hs.GetXaxis().GetXmax() ,val)
                self.lines[val].Draw('same')
                if val == 0:
                    self.lines[val].SetLineStyle(self.hlines_styles.get(val, 1))
                else:
                    self.lines[val].SetLineStyle(self.hlines_styles.get(val, 7))
                self.lines[val].SetLineColor(self.hlines_colors.get(val, r.kBlack))
                self.lines[val].SetLineWidth(self.hlines_width.get(val, 1))
                
        if self.vlines:
            self.lines2 = {}
            for val in self.vlines:
                if self.kind.lower() in ('g', 'graph', 'graphs'):
                    self.lines2[val] = r.TLine(val,self.hs.GetYaxis().GetXmin(),val,self.hs.GetYaxis().GetXmax())
                else:
                    r.gPad.Update()
                    self.lines2[val] = r.TLine(val,0, val, r.gPad.GetUymax())
                self.lines2[val].Draw('same')
                if val == 0:
                    self.lines2[val].SetLineStyle(self.vlines_styles.get(val, 1))
                else:
                    self.lines2[val].SetLineStyle(self.vlines_styles.get(val, 1))
                self.lines2[val].SetLineColor(self.vlines_colors.get(val, r.kBlack))
                self.lines2[val].SetLineWidth(self.vlines_width.get(val, 1))
