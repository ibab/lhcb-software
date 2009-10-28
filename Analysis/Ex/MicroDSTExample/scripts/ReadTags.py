import sys, getopt
from math import sqrt, fabs
from GaudiConf.Configuration import *
from AnalysisPython import Helpers, Functors, DebugMCAssoc
from Gaudi.Configuration import EventSelector
from GaudiPython.Bindings   import AppMgr
from GaudiPython.Bindings import gbl, AppMgr, Helper
#from GaudiPython.HistoUtils import HistoFile
from AnalysisPython.HistoUtils import book, fill , HistoFile
from GaudiKernel import SystemOfUnits, PhysicalConstants
from Configurables import MCMatchObjP2MCRelator
aida2root = gbl.Gaudi.Utils.Aida2ROOT.aida2root

import PartProp.Service
#==============================================================================
def safeFill(histo, value) :
    if value : fill(histo, value)

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
    
locationRoot = '/Event/MicroDST'
microDSTFile = ['']
histoFileName = "tags.root"
histoFile = HistoFile(histoFileName)
opts, args = getopt.getopt(sys.argv[1:], "i:r:h", ["input=", "root=", "help"])

for o, a in opts:
    if o in ("-h", "--help"):
        printHelp()
    elif o in ("-i", "--input"):
        microDSTFile=a
    elif o in ("-r", "--root"):
        locationRoot = a

# get rid of some spam
EventSelector().PrintFreq=100

from MicroDSTExample.Selections import SeqBs2Jpsi2MuMuPhi2KK
selSequence = SeqBs2Jpsi2MuMuPhi2KK.SeqBs2Jpsi2MuMuPhi2KK
mainLocation = selSequence.outputLocation()

# set up some useful paths of locations on the MicroDST
flavTagPath = locationRoot + "/" + mainLocation + "/FlavourTags"
lhcbApp = LHCbApp()
lhcbApp.DDDBtag = 'default'
lhcbApp.CondDBtag = 'default'

appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
appMgr.initialize()
appMgr.ExtSvc += ['LHCb::ParticlePropertySvc']
appMgr.HistogramPersistency = "ROOT"

evtSvc = appMgr.evtSvc()
toolSvc = appMgr.toolsvc()
evtSel = appMgr.evtSel()
nextEvent = Functors.NextEvent(appMgr)
pp = Functors.PartPropSvc(appMgr)
ppSvc = pp
particleNameFunc = Functors.ParticleName(ppSvc)

# open a DST or MicroDST
evtSel.open(microDSTFile)

histoPath = "MicroDST/Histos/"

omegaPlots = {}
# flavour tagging plot: plot mistag for each category.
for cat in range(0,6) :
    omegaPlots[cat] = book(histoPath+"FlavourTags/MisTagCat"+str(cat),
                           "Mis-tag fraction category "+ str(cat),
                           100, 0., 1.)


print "Looking for FlavourTags in ", flavTagPath
while ( nextEvent() ) :
    flavTags = evtSvc[flavTagPath]
    if flavTags :
        for tag in flavTags :
            print "Found a ", type(tag)
            safeFill( omegaPlots[tag.category()], tag.omega() )
            taggers = tag.taggers()
            if (taggers.size() > 0 ) :
                print "\n"
            for tagger in taggers :
                print "decision ", tagger.decision(), " type ", tagger.type(), " omega ", tagger.omega()
                parts = tagger.taggerParts()
                if parts :
                    print "Found ", parts.size(), " Tagger Particles"
                    for p in parts :
                        print "Particle ", particleNameFunc(p), " ", type(p), " ", p.parent().registry().identifier()
                else :
                    print "Found no Tagger Particles!!!"
