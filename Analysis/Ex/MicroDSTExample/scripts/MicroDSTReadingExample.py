import sys, getopt
from math import sqrt, fabs
from GaudiConf.Configuration import *
from MicroDSTExample import Helpers, Functors, Debug
from Gaudi.Configuration import EventSelector
from GaudiPython.Bindings   import AppMgr
from GaudiPython.Bindings import gbl, AppMgr, Helper
#from GaudiPython.HistoUtils import HistoFile
from MicroDSTExample.HistoUtils import book, fill , HistoFile
from ROOT import TCanvas, TH1D, Double
from GaudiKernel import SystemOfUnits, PhysicalConstants
from Configurables import MCMatchObjP2MCRelator
aida2root = gbl.Gaudi.Utils.Aida2ROOT.aida2root

import PartProp.Service
#==============================================================================
def safeFill(histo, value) :
    if value != None : fill(histo, value)

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
histoFileName = selection+"_mDST.root"
histoFile = HistoFile(histoFileName)
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
selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'
particle2mcPath = selectionPath + '/P2MCPRelations'
stdVertexAssocPath = selectionPath + '/Particle2VertexRelations'
refitVertexAssocPath = selectionPath + '/P2ReFitPVSortedRelations'
mcParticlePath = locationRoot+'/MC/Particles'
flavTagPath = selectionPath + "/Tagging"
pvLocation = locationRoot+"/Rec/Vertex/Primary"

# configure the new Particle->MCPatricle associator tool MCMatchObjP2MCRelator
myP2MCP = MCMatchObjP2MCRelator()
myP2MCP.OutputLevel=4
myP2MCP.MCParticleDefaultLocation = mcParticlePath
myP2MCP.RelTableLocations = [particle2mcPath]

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

# get an instance of PropertimeFitter
properTimeFitter = toolSvc.create('PropertimeFitter',
                                  interface='ILifetimeFitter')

# get an instance of MCMatchObjP2MCRelator
MCAssocTool = toolSvc.create('MCMatchObjP2MCRelator',
                             interface='IP2MCP')

# open a DST or MicroDST
evtSel.open(microDSTFile)

nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0

# some functors to aid in histogram naming and axis setting
particleNameFunc = Functors.ParticleName(ppSvc)

# List of "interesting" particle PIDs
# Add PID of anything yo uthink might be interesting!
interestingParticles = [531,-531, 333, 443, 511, -511, 321, -321, 211, -211, -13, 13]

histoPath = "MicroDST/Histos/"
massPlots = {}
massResPlots = {}
ptPlots = {}

for pid in interestingParticles :
    pp=ppSvc(pid)
    particleName = pp.particle()
    print "Booking plots for particle ", particleName, ", PID ", pid 
    pdgMass = pp.mass()
    massPlots[pid] = book ( histoPath+"Particle/M_"+particleName,
                             particleName + " mass",
                             100, pdgMass-100., pdgMass+100,)
    massResPlots[pid] = book( histoPath+"Particle/MResol_"+particleName,
                               particleName + " mass resolution",
                               100, -100., 100,)
    ptPlots[pid] = book(histoPath+"Particle/Pt_"+particleName,
                        particleName+ "Pt",100, 0., 20000.)


# proper time plots
propTimePlot = book(histoPath+"Particle/PropRes",
                    "Proper time",
                    100, -2., 5.)
refitPropTimePlot = book(histoPath+"Particle/RefitPropTime",
                         "Re-fitted PV proper time",
                         100, -2., 5.)

propTimeResPlot = book(histoPath+"Particle/PropTimeRes",
                       "Proper time resolution",
                       100, -0.2,0.5)
refitPropTimeResPlot = book(histoPath+"Particle/RefitPropTimeRes",
                            "Re-fitted PV proper time resolution",
                            100, -0.2,0.5)
# PV plots
vertexZ =  book( histoPath+"PV/PVz", "xxx xx PVz",
                 50, -100., 100.)
nPVPlot = book( histoPath+"PV/nPV",
                "# of primary vertices",
                5, -0.5, 4.5 )
bestVertexZ =  book(histoPath+"BestPV/PVz",
                    "best PVz",
                    50, -100.,100.)
omegaPlots = {}
# flavour tagging plot: plot mistag for each category.
for cat in range(0,6) :
    omegaPlots[cat] = book(histoPath+"FlavourTags/MisTagCat"+str(cat),
                           "Mis-tag fraction category "+ str(cat),
                           100, 0., 1.)

# a little functor to print out MC association trees
assocCounter = Debug.AssocTreeDebugger(MCAssocTool, particleNameFunc)

# we want mass and Pt plots to be made for all particles, so we need to make a
# functor X such that calling X(particle) results in filling the plot with
# the relevant information
# the mass resolution plotter requires an MC associator so has to be defined
# further down inside the event loop.
ptPlotter = Functors.HistoPlotter(ptPlots, gbl.LHCb.Particle.pt, Helpers.pid)
massPlotter = Functors.HistoPlotter(massPlots, Functors.Mass(), Helpers.pid)
# set the functor in the event loop
massResPlotter = Functors.HistoPlotter(massResPlots, None, Helpers.pid)
tauPlotter = Functors.HistoPlotter(propTimePlot, functor = None)
refitTauPlotter = Functors.HistoPlotter(refitPropTimePlot, functor = None)
tauResPlotter = Functors.HistoPlotter(propTimeResPlot, functor = None)
refitTauResPlotter = Functors.HistoPlotter(refitPropTimeResPlot, functor = None)

# create some recursive container loopers that use Particle.daughters() to get
# a container from an element in a container
recursionFunc = gbl.LHCb.Particle.daughters
ptRecursiveLoop = Functors.ContainerRecursiveLoop(ptPlotter, recursionFunc)
massRecursiveLoop = Functors.ContainerRecursiveLoop(massPlotter, recursionFunc)
massResRecursiveLoop = Functors.ContainerRecursiveLoop(massResPlotter,
                                                       recursionFunc)

propTimeLoop = Functors.ContainerLoop( tauPlotter )
refitPropTimeLoop = Functors.ContainerLoop( refitTauPlotter )
propTimeResLoop = Functors.ContainerLoop( tauResPlotter )
refitPropTimeResLoop = Functors.ContainerLoop( refitTauResPlotter )

while ( nextEvent() ) :
    nEvents+=1
    PVs = evtSvc[pvLocation]
    nPrimaryVertices += PVs.size()
    safeFill( nPVPlot, PVs.size() )
    mcParts = evtSvc[mcParticlePath]
    if mcParts != None : nMCEvents += 1
    
    for PV in PVs : safeFill(vertexZ, PV.position().z() )
    
    particles = evtSvc[particlePath]
    flavTags = evtSvc[flavTagPath]
    for tag in flavTags :
        safeFill( omegaPlots[tag.category()], tag.omega() )

    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        bestVertexFun = Functors.BestTo(evtSvc[stdVertexAssocPath])
        refitBestVertexFun = Functors.BestTo(evtSvc[refitVertexAssocPath])
        MCAssocFun = Functors.MCAssociator(MCAssocTool, verbose=False)

        massResPlotter.functor = Functors.MassRes(MCAssocFun)

        # iterative looping over particles and their descentants and their
        # descendants descendants and their .....

        ptRecursiveLoop(particles)
        massRecursiveLoop(particles)
        massResRecursiveLoop(particles)

        # functors to get rec and MC proper time for standard and re-fitted
        # primary vertices
        tauFunc = Functors.PropTime(bestVertexFun, properTimeFitter)
        refitTauFunc = Functors.PropTime(refitBestVertexFun, properTimeFitter)
        mcTauFunc = Helpers.properTimeMC

        # functors to get proper time resolution for standard and re-fitted
        # primary vertices
        tauResFunc = Functors.TauRes(tauFunc, mcTauFunc, MCAssocFun)
        refitTauResFunc = Functors.TauRes(refitTauFunc, mcTauFunc, MCAssocFun)

        # set the plotter's functors
        tauPlotter.functor = tauFunc
        refitTauPlotter.functor = refitTauFunc
        tauResPlotter.functor = tauResFunc
        refitTauResPlotter.functor = refitTauResFunc

        # perform simple loops on particle containers, filling histograms for
        # each particle
        propTimeLoop(particles)
        refitPropTimeLoop(particles)

        propTimeResLoop(particles)
        refitPropTimeResLoop(particles)
    
print "Found MC info in ", nMCEvents, "/", nEvents, " events"
print "Found Reco info in ", nRecEvents, "/", nEvents, "events"
print "Found ", nPrimaryVertices, " PVs in ", nEvents, "events"
print "Found ", nParticles, " B candidates in ", nEvents, "events"

print "massPlots.keys() = ", massPlots.keys()
print "massResPlots.keys() = ", massResPlots.keys()
print "omegaPlots.keys() = ", omegaPlots.keys()
print "propTimeResPlot: ", propTimeResPlot
print "reFitPropTimeResPlot: ", refitPropTimeResPlot

print "Saving histograms to file ", histoFileName

for pid in interestingParticles :
    histoFile.save(massPlots[pid],
                   massResPlots[pid],
                   ptPlots[pid])
    
histoFile.save(propTimePlot,
               propTimeResPlot,
               refitPropTimePlot,
               refitPropTimeResPlot,
               vertexZ,
               bestVertexZ,
               nPVPlot)

histoFile.close()
