#!/usr/bin/env python
__author__  = "Wouter Hulsbergen"
__version__ = ""

#apparently this needs to happen before Gaudi/Pool is loaded
from ROOT import TGraph, TCanvas

import os,time
import CondDBUI
import CondDBUI.Admin
from PyCool import cool

from Gaudi.Configuration import *

def intTime( time, default ):
    try:
        inttime = cool.ValidityKey(time)
    except ValueError:
        inttime = CondDBUI.Admin.timeToValKey( time, default)
    return inttime

from optparse import OptionParser
parser = OptionParser(usage = "%prog [options] database element-name",
                      version = __version__,
                      description =
                      """This script produces a plot of the value of an alignment condition versus time. For example\n
                      drawGlobalPosition.py /afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/IOVTest/LocalDDDB.db /dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight""")
parser.add_option("-s", "--sqlitefilename",
                  dest="sqlitefilename", type="string",
                  help="Name of the an LHCBCOND sqlitefile to be loaded on top of the default connection [default: %default]",
                  default="None")
parser.add_option("-T", "--tag",
                  action="store_true", dest="tag",
                  help="tag for the snapshot [default: %default]",
                  default="HEAD")
parser.add_option("-t", "--delta-time",
                  dest="dtime", type="int",
                  help="time interval for getting values in seconds [default: %default]",
                      default=60)
parser.add_option("-S", "--since",dest="since",
                  type="string",
                  help="begin of interval of validity"
                  "Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]",
                  default="2009-01-01")
parser.add_option("-U", "--until",dest="until",
                  type="string",
                  help="end of interval of validity"
                  "Format: YYYY-MM-DD[_HH:MM[:SS.SSS]][UTC]",
                  default="2009-12-31")
(opts, args) = parser.parse_args()

database    = args[0]
elementname = args[1]
    
# check arguments
if len(args) != 2:
    parser.error("not enough arguments. Try with --help.")
    

# we'll count everything in seconds
nsPerSecond  = 1000000000
from PyCool import cool
from GaudiPython.Bindings import gbl
timebegin = intTime(opts.since,cool.ValidityKeyMin)/nsPerSecond
timeend   = intTime(opts.until,cool.ValidityKeyMax)/nsPerSecond

print "From:  ", opts.since, " --> ", gbl.Gaudi.Time(timebegin,0).format(True,"%c")
print "Until: ", opts.until, " --> ", gbl.Gaudi.Time(timeend,0).format(True,"%c")

timeinterval = opts.dtime 
npoints = (timeend-timebegin)/timeinterval

# 1 september:  1231459200
# 15 september: 1268092800

# root uses a clock that starts in 1995
roottimeoffset = 788918400

from Configurables import LHCbApp, HistogramPersistencySvc, ApplicationMgr,UpdateManagerSvc
LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "head-20090928"

# to load a layer
from Configurables import CondDBAccessSvc, CondDB
#aligncond = CondDBAccessSvc( 'aligncond' )
#aligncond.ConnectionString = 'sqlite_file:/afs/cern.ch/lhcb/group/tracking/vol7/wouter/DB/IOVTest/VeloIOVTest.db/LHCBCOND'
#CondDB().addLayer( aligncond )

# load an entire snapshot
cdb = CondDB()
cdb.PartitionConnectionString["LHCBCOND"] = "sqlite_file:" + database + "/LHCBCOND"
LHCbApp().CondDBtag = "HEAD"

HistogramPersistencySvc().OutputFile = 'histograms.root'
appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'myBrunel' )

import GaudiPython
from GaudiPython.Bindings import gbl

appMgr = GaudiPython.AppMgr()
det = appMgr.detSvc()

element = det[elementname] 
#'/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight']
#element = det['/dd/Structure/LHCb/AfterMagnetRegion/T/IT/Station2/TopBox/LayerX1']

appMgr.createSvc('UpdateManagerSvc')
updatemgrsvc = appMgr.service(name = 'UpdateManagerSvc', interface = 'IUpdateManagerSvc')
detdatasvc = updatemgrsvc.detDataSvc()

    
xgraph = TGraph(npoints)
origin = gbl.Gaudi.XYZPoint()
timeiter = timebegin
ipoint =0
xmax = 0.

while timeiter < timeend:
    gauditime = gbl.Gaudi.Time(int(timeiter),0)
    detdatasvc.setEventTime( gauditime )
    updatemgrsvc.newEvent()
    updatemgrsvc.newEvent( gauditime )
    #updatemgrsvc.update()
    globalcenter = element.geometry().toGlobal( origin )
    #print timeiter, ' ', \
    #      gauditime.format(True,"%D %T"), " --> ", \
    #      '(',  globalcenter.x() , ",", globalcenter.y(), ",", globalcenter.z(), ")"
    #print timeiter, gauditime.nanoformat(True), gauditime.year(False)
    timeiter += timeinterval
    
    xgraph.SetPoint( ipoint, timeiter - roottimeoffset, globalcenter.x() )
    if abs( globalcenter.x() ) > xmax:
        xmax = abs( globalcenter.x() )
    ipoint +=1

canvas = TCanvas('c1')
frame = canvas.DrawFrame(timebegin- roottimeoffset,-1.1*xmax,timeend- roottimeoffset,1.1*xmax)
frame.SetTitle(element.name())
frame.GetXaxis().SetTimeDisplay(True)

period = timeend - timebegin
if period < 25*3600:
    frame.GetXaxis().SetTimeFormat("%H:%M")
else:
    frame.GetXaxis().SetTimeFormat("%d/%m")
xgraph.SetMarkerStyle(20)    
xgraph.SetMarkerSize(0.5)    
xgraph.Draw("P")

canvas.Print('globalposition.eps')

