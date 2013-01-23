# coding=utf-8
"""
Module with functions to create pdf pages using matplotlib
"""
__author__ = "Pieter David <pieter.david@nikhef.nl>"
__date__   = "November 2012"

__all__  = ( "ElementGroups"
           , "drawTimeTrends"
           , "folders"
           )

import os
import re
import logging
import datetime
import math

import matplotlib as mpl
mpl.rcParams["backend"] = "PDF"
import matplotlib.pyplot as plt
import matplotlib.dates
import matplotlib.ticker
from matplotlib.figure import SubplotParams

class PlotRegion(object):
    """
    A class representing a "plot region" on a page (axes)

    Knows how to make one plot.
    @see the subclasses for time trends (TrendPlotRegion) and diffs (DiffPlotRegion)
    """
    def __init__(self, name, var, parent, axes):
        self.name = name
        self.var = var
        self.parent = parent

        self.axes = axes

    def getFullName(self):
        return "".join((self.parent.getFullName(), self.name))

    def draw(self):
        pass

    def addAlignment(self, parValues, **drawOptions):
        pass
    
    def addReference(self, parValues, **drawOptions):
        pass

def roundUp( num, resolution=1.0 ):
    return math.ceil( num / resolution )*resolution

def roundDown( num, resolution=1.0 ):
    return math.floor( num / resolution )*resolution

class TrendPlotRegion(PlotRegion):
    DefaultFormats = { "MagUp"   : "r-"
                     , "MagDown" : "b-"
                     , "MagOff"  : "g-"
                     , "TS"      : { "facecolor" : "k" , "alpha" : 0.05 , "lw" : 0.0 }
                     , "MD"      : { "facecolor" : "k" , "alpha" : 0.05 , "lw" : 0.0 }
                     , "VDM"     : { "facecolor" : "k" , "alpha" : 0.05 , "lw" : 0.0 }
                     }
    def __init__(self, name, var, parent, axes, periods=None):
        self.periods  = periods
        self.mn = None
        self.mx = None
        super(TrendPlotRegion, self).__init__(name, var, parent, axes)

    def draw(self):
        super(TrendPlotRegion, self).draw()

        res = 1.0
        if self.var.startswith("T"):
            res = 0.1
        elif self.var.startswith("R"):
            res = 0.01

        dmnmx = self.mx-self.mn
        dmin = roundDown( self.mn-.1*dmnmx, res )
        dmax = roundUp( self.mx+.1*dmnmx, res )
        self.axes.set_ylim( dmin, dmax )
        if dmnmx < 5.*res:
            self.axes.yaxis.set_major_locator(matplotlib.ticker.MultipleLocator(res))
        elif dmnmx < 100.*res:
            self.axes.yaxis.set_minor_locator(matplotlib.ticker.MultipleLocator(res))

        if self.var.startswith("T"):
            self.axes.set_ylabel("%s (mm)" % self.var)
        elif self.var.startswith("R"):
            self.axes.set_ylabel("%s (mrad)" % self.var)
        else:
            self.axes.set_ylabel(self.var)

        self.axes.xaxis.set_minor_locator( matplotlib.dates.MonthLocator() )

        for period in self.periods:
            if not period.status.needAlignment:
                self.axes.axvspan( period.startTime, period.endTime, **(TrendPlotRegion.DefaultFormats[period.status.name]))
        self.axes.set_title(self.name)

    def addAlignment(self, parValues, addStats=False, **drawOptions):
        # rad -> mrad
        if self.var.startswith("R"):
            parValues = list( 1000.*p for p in parValues )

        if not "solid_capstyle" in drawOptions:
            drawOptions["solid_capstyle"] = "butt"

        if not len(parValues) == sum(1 for p in self.periods if p.status.needAlignment):
            logging.error("Too few parameter values, check!!")
            return
        curPar = None
        curParIt = iter(parValues)
        for period in self.periods:
            if period.status.needAlignment:
                curPar = curParIt.next()
                fmt = drawOptions.pop("fmt", TrendPlotRegion.DefaultFormats[period.status.name])
                self.axes.plot( (period.startTime, period.endTime), (curPar, curPar), fmt, **drawOptions )

        if addStats:
            avg = sum( p for p in parValues ) / len(parValues)
            rms = math.sqrt( sum( (p-avg)*(p-avg) for p in parValues ) / len(parValues) )
            self.axes.text( 1., 1.02, u"%.02f±%.02f" % (round(avg,2), round(rms,2)), ha="right", va="baseline", transform=self.axes.transAxes )

        # axis limits
        if self.mn is not None and self.mx is not None:
            self.mn, self.mx = min( min(parValues), self.mn ), max( max(parValues), self.mx )
        else:
            self.mn, self.mx = min(parValues), max(parValues)

    def addReference(self, parValue, **drawOptions):
        # rad -> mrad
        if self.var.startswith("R"):
            parValue = 1000.*parValue

        drawOptions["alpha"] = drawOptions.get("alpha", 0.5)
        drawOptions["color"] = drawOptions.get("color", "k")
        self.axes.axhline( parValue, **drawOptions )

        # axis limits
        if self.mn is not None and self.mx is not None:
            self.mn, self.mx = min( parValue, self.mn ), max( parValue, self.mx )
        else:
            self.mn, self.mx = parValue, parValue

class DiffPlotRegion(PlotRegion):
    def __init__(self, name, var, parent, axes):
        self.mn = None
        self.mx = None
        self.labels = list()
        self.refLabels = list()
        self._x = -1.
        self._xr = 0.
        super(DiffPlotRegion, self).__init__(name, var, parent, axes)

    def draw(self):
        super(DiffPlotRegion, self).draw()

        if self.var.startswith("T"):
            self.axes.set_ylabel("%s (mm)" % self.var)
        elif self.var.startswith("R"):
            self.axes.set_ylabel("%s (mrad)" % self.var)
        else:
            self.axes.set_ylabel(self.var)

        self.axes.set_xlim(self._xr-.5, self._x+.5)
        self.axes.set_xticks(range(-len(self.refLabels), len(self.labels)))
        self.axes.set_xticklabels(list(l for l in reversed(self.refLabels)) + self.labels)

        self.axes.set_title(self.name)

    def addAlignment(self, parValue, label=None, draw="P", **drawOptions):
        # rad -> mrad
        if self.var.startswith("R"):
            parValue = 1000.*parValue

        if draw == "P":
            self.labels.append(label)
            self._x += 1.
            self.axes.plot( ( self._x, ), ( parValue, ), drawOptions.pop("fmt", "k."), **drawOptions )
        elif draw == "L":
            self.axes.axhline( parValue, **drawOptions )

    def addReference(self, parValue, label=None, draw="L", **drawOptions):
        # rad -> mrad
        if self.var.startswith("R"):
            parValue = 1000.*parValue

        if draw == "P":
            self.refLabels.append(label)
            self._xr -= 1.
            self.axes.plot( ( self._xr, ), ( parValue, ), drawOptions.pop("fmt", "k."), **drawOptions )
        elif draw == "L":
            self.axes.axhline( parValue, **drawOptions )

class Page(object):
    """
    A class representing a "page" (maps to a TCanvas)

    Container of plot region, main task is to delegate things to the right one.
    Additionally handles output to a file
    """
    def __init__(self, name, parent, nRows, nCols):
        """
        Constructor
        """
        self.name = name
        self.parent = parent

        self.nRows = nRows
        self.nCols = nCols
        self.fig = plt.figure(figsize=(11.6929134, 8.26771654), subplotpars=SubplotParams(wspace=0.35, left=0.1, bottom=0.2, right=0.98))
        self._l = 0

        self.regions = dict()

    def getFullName(self):
        return "".join((self.parent.getFullName(), self.name))

    def addRegion(self, name, var, **kwargs):
        self._l += 1
        self.regions[name] = kwargs.pop("RegionType", PlotRegion)(name, var, self, self.fig.add_subplot(self.nRows, self.nCols, self._l), **kwargs)
        topInCol = self.fig.add_subplot(self.nRows, self.nCols, ((self._l-1) % self.nCols)+1 )
        self.regions[name].axes.sharex = topInCol

    def __getitem__(self, name):
        return self.regions.__getitem__(name)

    def save(self, outputFileName):
        self.fig.suptitle(self.name)
        for reg in self.regions.itervalues():
            reg.draw()
        self.fig.autofmt_xdate()
        self.fig.savefig(outputFileName)

class Folder(object):
    """
    A class representing a folder - a set of pages

    Container of pages, may be used to create grouped output
    """
    def __init__(self, name):
        """
        Constructor
        """
        self.name = name
        self.pages = dict()

    def getFullName(self):
        return self.name

    def addPage(self, name, nRows, nCols):
        self.pages[name] = Page(name, self, nRows, nCols)

    def __getitem__(self, name):
        return self.pages.__getitem__(name)

    def save(self, outputDir=".", extension="pdf"):
        outputDirFull = os.path.join(*([ outputDir ] + self.name.split(".")))
        if not os.path.isdir(outputDirFull):
            os.makedirs(outputDirFull)
        for n, p in self.pages.iteritems():
            logging.info("Saving page %s to directory %s (%s)" % ( self.getFullName(), outputDirFull, ".".join((n,extension)) ))
            p.save(os.path.join(outputDirFull, ".".join((n, extension))))

class ElementGroup(object):
    """
    Helper class to figure out from the element name if we want to draw it
    and to which page it should go (match method returns the right page or None).
    """
    def __init__(self, expr, nRows, nCols):
        self.regex = re.compile(expr)
        self.nRows = nRows
        self.nCols = nCols
    def match(self, elementName):
        m = self.regex.match(elementName)
        if m is None:
            return None
        else:
            return ( m.group("page").replace("/", "_"), m.group("elm").replace("/", "_") )

ElementGroups = { "TT.Layers"       : ElementGroup("(?P<page>TT)/TT[ab]/(?P<elm>TT(?:aX|aU|bV|bX)Layer)$", 2, 2)
                , "TT.Modules"      : ElementGroup("TT/TT[ab]/(?P<page>TT(?:aX|aU|bV|bX)Layer/R[1-3])Module(?P<elm>[1-6][BT])$", 4, 3)
                , "IT.Boxes"        : ElementGroup("IT/(?P<page>Station[1-3])/(?P<elm>(?:ASide|CSide|Top|Bottom)Box)$", 2, 2)
                , "IT.Layers"       : ElementGroup("IT/(?P<page>Station[1-3]/(?:ASide|CSide|Top|Bottom)Box)/(?P<elm>(?:X1|U|V|X2)Layer)$", 2, 2)
                , "OT.CFrames"      : ElementGroup("OT/(?P<page>T[1-3]/(?:X1|U|V|X2))/(?P<elm>Q[0-3])$", 2, 2)
                , "OT.CFrameLayers" : ElementGroup("OT/(?P<page>T[1-3])/(?P<elm>(?:X1|U|V|X2)/Q[0-1])$", 4, 2)
                , "OT.Modules"      : ElementGroup("OT/(?P<page>T[1-3]/(?:X1|U|V|X2)/Q[0-3])/(?P<elm>M[1-9])$", 3, 3)
                }
# TODO implement an "ElementGroupFolder" that has ElementGroup, the regular expressions all methods below in its members

folders = {}

def getOrCreateFolder(folderName):
    folder = folders.get(folderName, None)
    if not folder:
        folder = Folder(folderName)
        folders[folderName] = folder
    return folder

def getRegion( folder, elmGroup, elmPageName, dof, RegionType=TrendPlotRegion, **kwargs ):
    pageName = "__".join((elmPageName[0], dof))
    regName = elmPageName[1]
    # get the page
    if pageName not in folder.pages:
        folder.addPage( pageName, elmGroup.nRows, elmGroup.nCols )
    page = folder[pageName]
    # get the region
    if regName not in page.regions:
        page.addRegion( regName, dof, RegionType=RegionType, **kwargs )
    return page[regName]

def drawTimeTrends( periods, elmGroupName, alignmentsWithIOVs, dofs, addStats=False ):
    folder = getOrCreateFolder(elmGroupName)
    elmGroup = ElementGroups[elmGroupName]
    elmDetName = elmGroupName.split(".")[0]

    logging.info("Adding time trends for %s of %s to the plots" % ( ", ".join(dofs), elmGroupName ))

    for geomName, elmPageName, matrixForAll in alignmentsWithIOVs.loopWithTimesAndValues(elmDetName, elmGroup, periods):
        for dof in dofs:
            region = getRegion(folder, elmGroup, elmPageName, dof, RegionType=TrendPlotRegion, periods=periods )
            logging.debug("Adding constants for element %s: %s to region %s (%s)" % ( geomName, [ getattr(align, dof) for align in matrixForAll ], region, region.name ))
            region.addAlignment( [ getattr(align, dof) for align in matrixForAll ], addStats=addStats )

def drawTimeTrendReference( periods, elmGroupName, alignmentsWithIOVs, dofs ):
    folder = getOrCreateFolder(elmGroupName)
    elmGroup = ElementGroups[elmGroupName]
    elmDetName = elmGroupName.split(".")[0]

    logging.info("Adding references for %s of %s to the plots" % ( ", ".join(dofs), elmGroupName ))

    for geomName, elmPageName, matrix in alignmentsWithIOVs.loopWithTimesAndValues(elmDetName, elmGroup, list(p for p in periods if p.status.needAlignment)[0:1]):
        for dof in dofs:
            region = getRegion(folder, elmGroup, elmPageName, dof, RegionType=TrendPlotRegion, periods=periods )
            region.addReference(getattr(matrix[0], dof))

def drawDiffAlignment( periods, elmGroupName, alignment, dofs, label, draw="P" ):
    folder = getOrCreateFolder(elmGroupName)
    elmGroup = ElementGroups[elmGroupName]
    elmDetName = elmGroupName.split(".")[0]

    logging.info("Adding alignment for %s of %s to the plots" % ( ", ".join(dofs), elmGroupName ))

    for geomName, elmPageName, matrix in alignment.loopWithTimesAndValues(elmDetName, elmGroup, periods ):
        for dof in dofs:
            region = getRegion(folder, elmGroup, elmPageName, dof, RegionType=DiffPlotRegion )
            region.addAlignment(getattr(matrix[0], dof), label=label)

def drawDiffReference( periods, elmGroupName, alignment, dofs, draw="P" ):
    folder = getOrCreateFolder(elmGroupName)
    elmGroup = ElementGroups[elmGroupName]
    elmDetName = elmGroupName.split(".")[0]

    logging.info("Adding reference for %s of %s to the plots" % ( ", ".join(dofs), elmGroupName ))

    for geomName, elmPageName, matrix in alignment.loopWithTimesAndValues(elmDetName, elmGroup, periods ):
        for dof in dofs:
            region = getRegion(folder, elmGroup, elmPageName, dof, RegionType=DiffPlotRegion )
            region.addReference(getattr(matrix[0], dof))
