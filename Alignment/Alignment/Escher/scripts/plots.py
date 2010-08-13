#!/usr/bin/env python
__author__  = "Nicola Chiapolini, Albert Bursche"
__version__ = "1.0"

from ROOT import *
from libPlots import *
from libInspect import *

#import GaudiPython 
#from Gaudi.Configuration import * 
#from Configurables import LHCbApp, ApplicationMgr

gROOT.SetStyle("Plain")
rootColors = [kBlack,kRed,kBlue,kGreen,kMagenta,kCyan,kYellow]
canvas = []
legend = []

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
    parser.add_option("-s", "--save", action="store_true", default=False, 
        dest="save", help="save plots to current directory"),
    parser.add_option("-t", "--filetype", type="string", default=".pdf", 
        dest="filetype", help="set format of save files")

    (opts, args) = parser.parse_args()

    plots = [ 
      ArrowPlot("TT-ZX","/TT.*Module[0-9]*(T|B)$",
	  PlotFunctions.zxShifts),
      ArrowPlot("TT-ZXlayers","/TT(?!.*(Module|Sensor).*)", 
	  PlotFunctions.zxShifts),
      ArrowPlot("TT-XY","/TT(?!T)(?!.*Sensor.*)",
          PlotFunctions.xyShifts),
      ArrowPlot("TT-ZY","/TT(?!T)(?!.*Sensor.*)",
          PlotFunctions.zyShifts),
      Plot("TT-ZtX","/TT(?!T)(?!.*Sensor.*)",
          lambda do:[do.Z,do.TX]),
      Plot("TTaX-XtX","/TTaX(?!T)(?!.*Sensor.*)",
          lambda do:[do.X,do.TX]),
      Plot("TTaU-XtX","/TTaU(?!T)(?!.*Sensor.*)",
          lambda do:[do.X,do.TX]),
      Plot("TTbV-XtX","/TTbV(?!T)(?!.*Sensor.*)",
          lambda do:[do.X,do.TX]),
      Plot("TTbX-XtX","/TTbX(?!T)(?!.*Sensor.*)",
          lambda do:[do.X,do.TX]),
      Plot("TT-TzTx","/TT(?!.*(Module|Sensor).*)", 
	  lambda do:[do.TZ,do.TX])
      ]

    run(["/dd/Structure/LHCb/BeforeMagnetRegion/TT"], opts, addToPlotsRecursive)

    map(drawPlot, plots)
    drawCombinedPlot([plots[0],plots[1]])
    drawCombinedPlot([plots[5],plots[6],plots[7],plots[8]])
    if (opts.save):
        savePlots(opts.filetype)


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
    legend.append(TLegend(0.15,0.7,0.3,0.9))
    
    i = 1
    axisPlot = None
    title = "combined" 
    for plot in plotList:
      title += "_"+plot.name
      legend[-1].AddEntry(plot.getGraph(), plot.name, "p")

      plot.getGraph().SetMarkerColor(rootColors[i])
      if i == 1:
	plot.draw("ap", labels)
	axisPlot = plot
      else:
        plot.draw("p same") 
      i = i + 1
    axisPlot.draw("p same")  # ensures that view range can be adjusted
    legend[-1].Draw();
    canvas[-1].SetTitle(title)


def savePlots(filetype):
    for c in canvas:
      if c != None:
	c.Print(c.GetTitle()+filetype)
    
main()
