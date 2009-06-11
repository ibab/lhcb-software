import sys, getopt
from math import sqrt, fabs
from GaudiConf.Configuration import *
from MicroDSTExample import Helpers, Functors, Debug
from Gaudi.Configuration import EventSelector
from GaudiPython.Bindings   import AppMgr
from GaudiPython.Bindings import gbl, AppMgr, Helper
from MicroDSTExample.HistoUtils import book, fill
from ROOT import TCanvas, TH1D, Double
from GaudiKernel import SystemOfUnits, PhysicalConstants
from Configurables import MCMatchObjP2MCRelator
aida2root = gbl.Gaudi.Utils.Aida2ROOT.aida2root

GenPlotter = Functors.GenericPlotter
import PartProp.Service
#==============================================================================
def EventDataPlots(data, plotter) :
    if (data != None):
        for d in data:
            plotter(d)
        return 1
    else :
        return 0
#==============================================================================
def printHelp():
    print "Usage: python -i MicroDSTReadingExample [options]"
    print "Options:\n"
    print "\t--input         Input MicroDST file"
    print "                  Default ''"
    print "\t--selection     DV Selection run to make the MicroDST."
    print "                  Default 'DC06selBs2JpsiPhi_unbiased'"
    print "\t--root          TES root of everything."
    print "                  Default 'Event/microDST'"
    
locationRoot = '/Event/microDST'
selection = 'DC06selBs2JpsiPhi_unbiased'
microDSTFile = ['']

opts, args = getopt.getopt(sys.argv[1:], "s:i:r:h", ["selection=","input=", "root=", "help"])

for o, a in opts:
    if o in ("-h", "--help"):
        printHelp()
    elif o in ("-i", "--input"):
        microDSTFile=a
    elif o in ("-s", "--selection"):
        selection = a
    elif o in ("-r", "--root"):
        locationRoot = a

# get rid of some spam
EventSelector().PrintFreq=100

# set up some useful paths of locations on the MicroDST
hltPath = locationRoot + "/Hlt/DecReports"
l0Path = locationRoot + "/Trig/L0/L0DUReport"
selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'

lhcbApp = LHCbApp()
lhcbApp.DDDBtag = 'default'
lhcbApp.CondDBtag = 'default'

appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$STDOPTS/LHCbApplication.opts',
                        '$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts',
                        '$DDDBROOT/options/DC06.opts'])
appMgr.initialize()
appMgr.ExtSvc += ['LHCb::ParticlePropertySvc']
appMgr.HistogramPersistency = "ROOT"

evtSvc = appMgr.evtSvc()
evtSel = appMgr.evtSel()
nextEvent = Helpers.NextEvent(appMgr)

# open a DST or MicroDST
evtSel.open(microDSTFile)

nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0

while ( nextEvent() ) :
    nEvents+=1
    print "Event ", nEvents, ". Getting HltDecReports from ", hltPath
    decReports = evtSvc[hltPath]
    if decReports != None :
        print "Found ", decReports.size(), " HLTDecReports!"
        names = decReports.decisionNames()
        print "names.size() = ", names.size()
        for name in names :
            report = decReports.decReport(name)
            if report != None :
                print "Found ", name, ": ", report.decision()
            else :
                print "ERROR: did not find ", name
    else :
        print "No HltDecReports found in ", hltPath

    l0du = evtSvc[l0Path]
    if l0du != None :
        print "Found L0DUReport, decision ", l0du.decision()
    else :
        print "No L0DUReport found in ", l0Path
        
    print "Event ", nEvents, ". Getting HltDecReports from ", hltPath
