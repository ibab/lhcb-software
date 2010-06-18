#!/usr/bin/env python
__author__  = "Nicola Chiapolini, Albert Bursche"
__version__ = "1.0"

from ROOT import *
from libPlots import *

import GaudiPython 
from Gaudi.Configuration import * 
from Configurables import LHCbApp, ApplicationMgr

gROOT.SetStyle("Plain")
canvas = []

def main():
    from optparse import OptionParser
    global plots

    parser = OptionParser(
	usage = "",
        version = __version__,
        description = """This script produces plots from a given alignment db."""
	)
    parser.add_option("-d", "--aligndb", action="append", dest="aligndb",
        help="path to sqlite-file with alignment database layer") 
    parser.add_option("-t", "--filetype", type="string", default=".pdf", 
        dest="filetype", help="set output format")

    (opts, args) = parser.parse_args()

    plots = [ 
      ArrowPlot("TT-ZX","/TT.*Module[0-9]*(T|B)$",
	  PlotFunctions.zxShifts),
      ArrowPlot("TT-ZXlayers","/TT(?!.*(Module|Sensor).*)", 
	  PlotFunctions.zxShifts),
      ArrowPlot("TT-XY","/TT(?!T)(?!.*Sensor.*)",
          PlotFunctions.xyShifts),
      Plot("TT-ZtX","/TT(?!T)(?!.*Sensor.*)",
          lambda do:[do.Z,do.TX]),
      ArrowPlot("TT-ZY","/TT(?!T)(?!.*Sensor.*)",
          PlotFunctions.zyShifts)
      ]

    run(["/dd/Structure/LHCb/BeforeMagnetRegion/TT"], opts, addToPlotsRecursive)
    map(drawPlot, plots)
    drawCombinedPlot([plots[0],plots[1]])
    savePlots(opts.filetype)


def run(startElementNames, opts, addFunction):
    global appMgr
    
    LHCbApp().DDDBtag   = "head-20100518"
    LHCbApp().CondDBtag = "head-20100518"

    if opts.aligndb:
      from Configurables import ( CondDB, CondDBAccessSvc )
      counter = 1
      for db in opts.aligndb:
	alignCond = CondDBAccessSvc( 'AlignCond' + str(counter) )
	alignCond.ConnectionString = 'sqlite_file:' + db + '/LHCBCOND'
	CondDB().addLayer( alignCond )
	counter += 1
    
    appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'myBrunel' )
    appMgr = GaudiPython.AppMgr()

    for element in startElementNames:
      startElement = getElementByName(element)
      addFunction(startElement)


def getElementByName(elementName):
    return appMgr.detSvc()[elementName]


def addToPlots(element):
    DetObj = ElementGeometry(element)
    map(lambda x:x.Fill(DetObj),plots)


def addToPlotsRecursive(element):
    addToPlots(element)
    
    for child in element.childIDetectorElements():
      addToPlotsRecursive(child)


def drawPlot(plot):
    canvas.append(TCanvas())
    canvas[-1].SetTitle(plot.name)
    plot.draw()


def drawCombinedPlot(plotList, labels = ""):
    canvas.append(TCanvas())
    
    i = 0
    axisPlot = None
    title = "combined" 
    for plot in plotList:
      title += "_"+plot.name
      plot.getGraph().SetMarkerColor(2+i)
      if i == 0:
	plot.draw("ap", labels)
	axisPlot = plot
      else:
        plot.draw("p same") 
      i = i + 1
    axisPlot.draw("p same")  # ensures that view range can be adjusted
    canvas[-1].SetTitle(title)


def savePlots(filetype):
    for c in canvas:
      if c != None:
	c.Print(c.GetTitle()+filetype)
    
main()
