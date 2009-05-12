# Imports and typedefs
from GaudiConf.Configuration import *
from Gaudi.Configuration import EventSelector
import sys, getopt
from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr, Helper
from math import sqrt, fabs
from GaudiKernel import SystemOfUnits, PhysicalConstants
from LHCbMath import XYZVector, XYZPoint
from Configurables import MCMatchObjP2MCRelator
from MicroDSTExample import Helpers, Functors, Debug
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
    print "                  Default /afs/cern.ch/lhcb/group/davinci/vol1/DEV/data/DV_v20r2/testBs2JpsiPhi_5Kevt_pythonConfig.dst"
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

# set up some useful paths
selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'
particle2mcPath = selectionPath + '/P2MCPRelations'
stdVertexAssocPath = selectionPath + '/Particle2VertexRelations'
refitVertexPath = selectionPath + '/RefittedVertices'
refitVertexAssocPath = selectionPath + '/Particle2ReFittedVertexRelations'
mcParticlePath = locationRoot+'/MC/Particles'
flavTagPath = selectionPath + "/Tagging"
pvLocation = locationRoot+"/Rec/Vertex/Primary"

# configure the new Particle->MCPatricle associator tool MCMatchObjP2MCRelator
myP2MCP = MCMatchObjP2MCRelator()
myP2MCP.OutputLevel=4
myP2MCP.MCParticleDefaultLocation = 'microDST/MC/Particles'
myP2MCP.RelTableLocations = [particle2mcPath]

lhcbApp = LHCbApp()
lhcbApp.DDDBtag = 'default'
lhcbApp.CondDBtag = 'default'

appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$STDOPTS/LHCbApplication.opts',
                        '$STDOPTS/SimDicts.opts',
                        '$DDDBROOT/options/DC06.opts'])
appMgr.initialize()

appMgr.ExtSvc += ['LHCb::ParticlePropertySvc']

eventLoop = Helpers.EventLoop(appMgr)

# get an instance of PropertimeFitter
properTimeFitter = eventLoop.tools.create('PropertimeFitter',
                                          interface='ILifetimeFitter')

# get an instance of MCMatchObjP2MCRelator
MCAssocTool = eventLoop.tools.create('MCMatchObjP2MCRelator',
                                     interface='IP2MCP')



eventLoop.sel.open(microDSTFile)
mcMassPlots = Helpers.Plots("MC mass")
mcPropTimePlots = Helpers.Plots("MC proper time")
massPlots = Helpers.Plots("Mass")
massResPlots = Helpers.Plots("Mass resolution")
propTimePlots = Helpers.Plots("Proper time")
refitPropTimePlots = Helpers.Plots("Re-fitted proper time")
propTimeResPlots = Helpers.Plots("Proper-time resolution")
omegaPlots = Helpers.Plots("Mis-tag fratcion")
nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0


# make some standard plots
nPVPlot = TH1D( "# of PVs", "# of primary vertices", 5, -0.5, 4.5 )
stdPropTimeResPlot = TH1D("tau-tau_MC", "tau-tau_MC", 100, -0.2,0.5)
refitPropTimeResPlot = TH1D("tau-tau_MC, re-fitted PV", "tau-tau_MC", 100, -0.2,0.5)
refitVertexZ =  TH1D("re-fit PVz", "re-fit PVz", 50, -100.,100.)
vertexZ =  TH1D("PVz", "PVz", 50, -100.,100.)
bestVertexZ =  TH1D("best PVz", "best PVz", 50, -100.,100.)

pidMassFunc = Functors.PIDMass(eventLoop.partProp)
pidMassHistoAtts = Helpers.AdaptiveHistoAttributes(100,
                                                     -100.,
                                                     100.,
                                                     pidMassFunc)
mcPIDMassHistoAtts = Helpers.AdaptiveHistoAttributes(100,
                                                       -5.,
                                                       5.,
                                                       pidMassFunc)
tauHistoAtts = Helpers.HistoAttributes(100, -2., 5.)
particleNameFunc = Functors.ParticleName(eventLoop.partProp)
particleMassPlotter = GenPlotter(massPlots,
                                 Functors.Mass(),
                                 Helpers.pid,
                                 particleNameFunc,
                                 pidMassHistoAtts,
                                 "mass")
# MC plots

mcMassPlotter = GenPlotter(mcMassPlots,
                           Functors.Mass(),
                           Helpers.pid,
                           particleNameFunc,
                           mcPIDMassHistoAtts,
                           "MC mass")
mcPropTimePlotter = GenPlotter(mcPropTimePlots,
                               Helpers.properTimeMC,
                               Helpers.pid,
                               particleNameFunc,
                               tauHistoAtts,
                               "MC tau")
# flavour tagging plot: plot mistag for each category.
tagOmegaPlotter = GenPlotter(omegaPlots,
                             Helpers.omega,
                             Helpers.category,
                             Functors.DefaultName("BTagging"),
                             Helpers.HistoAttributes(100, 0.,1.),
                             "Mis-tag fraction per category")

assocCounter = Debug.AssocTreeDebugger(MCAssocTool, particleNameFunc)

while (eventLoop.nextEvent() ) :
#    evt.dump()
    nEvents+=1
    nPrimaryVertices += eventLoop.countAndPlotNEntries(pvLocation, nPVPlot)
    mcParts = eventLoop.getEvtStuff(mcParticlePath)
    PVs = eventLoop.getEvtStuff(pvLocation)
    for PV in PVs : vertexZ.Fill(PV.position().z())
    
    nMCEvents += EventDataPlots(mcParts, mcMassPlotter)
    EventDataPlots(mcParts, mcPropTimePlotter)
    particles = eventLoop.getEvtStuff(particlePath)
    flavTags = eventLoop.getEvtStuff(flavTagPath)
    EventDataPlots(flavTags, tagOmegaPlotter)

    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        stdBestVertexAssoc = eventLoop.getEvtStuff(stdVertexAssocPath)
        bestVertexFun = Functors.BestVertex(stdBestVertexAssoc)
        refitBestVertexAssoc = eventLoop.getEvtStuff(refitVertexAssocPath)
        bestRefitVertexFun = Functors.BestVertex(refitBestVertexAssoc)
        p2MCPTable = eventLoop.getEvtStuff(particle2mcPath)
#        MCAssocFun = Functors.AssocMCPFromTable(p2MCPTable)
        MCAssocFun = Functors.MCAssociator(MCAssocTool, verbose=False)
        massResFunc = Functors.MassRes(MCAssocFun)
        particleMassResPlotter = GenPlotter(massResPlots,
                                            massResFunc,
                                            Helpers.pid,
                                            particleNameFunc,
                                            Helpers.HistoAttributes(100,-100., 100.),
                                            "rec mass - MC mass")
        Helpers.particleTreeLoop(particles, particleMassPlotter)
        Helpers.particleTreeLoop(particles, particleMassResPlotter)
        Helpers.particleTreeLoop(particles, assocCounter)
        stdTauFunc = Functors.PropTime(bestVertexFun,
                                       properTimeFitter)
        tauPlotter = GenPlotter(propTimePlots,
                                stdTauFunc,
                                Helpers.pid,
                                particleNameFunc,
                                tauHistoAtts,
                                "proper time")

        reFitPVTauFunc = Functors.PropTime(bestRefitVertexFun,
                                           properTimeFitter)

        reFitPVTauPlotter = GenPlotter(refitPropTimePlots,
                                       reFitPVTauFunc,
                                       Helpers.pid,
                                       particleNameFunc,
                                       tauHistoAtts,
                                       "re-fitted PV proper time")

        EventDataPlots(particles, reFitPVTauPlotter)
        EventDataPlots(particles, tauPlotter)

        for p in particles:
            stdVertex = bestVertexFun(p)
            if stdVertex!=None : bestVertexZ.Fill(stdVertex.position().z())
            stdPropTime = stdTauFunc(p)
            refitVertex = bestRefitVertexFun(p)
            pMom = p.momentum()
            refitVertices = eventLoop.getEvtStuff(refitVertexPath)
            if refitVertices != None :
                for rv in refitVertices :
                    if rv!=None : refitVertexZ.Fill(rv.position().z())
            if (refitVertex != None) :
                refitPropTime = reFitPVTauFunc(p)
            else :
                refitPropTime = -9999.
            assocMCPart = MCAssocFun(p)
            if (assocMCPart != None) :
                MCPropTime = Helpers.properTimeMC(assocMCPart)
                stdPropTimeRes = stdPropTime-MCPropTime
                stdPropTimeResPlot.Fill(stdPropTimeRes)
                refitProtTimeRes = refitPropTime-MCPropTime
                refitPropTimeResPlot.Fill(refitProtTimeRes)
    
print "Found MC info in ", nMCEvents, "/", nEvents, " events"
print "Found Reco info in ", nRecEvents, "/", nEvents, "events"
print "Found ", nPrimaryVertices, " PVs in ", nEvents, "events"
print "Found ", nParticles, " B candidates in ", nEvents, "events"

print "massPlots.plots.keys() = ", massPlots.plots.keys()
print "mcMassPlots.plots.keys() = ", mcMassPlots.plots.keys()
print "mcPropTimePlots.plots.keys() = ", mcPropTimePlots.plots.keys()
print "massResPlots.plots.keys() = ", massResPlots.plots.keys()
print "propTimePlots.plots.keys() = ", propTimePlots.plots.keys()
print "refitPropTimePlots.plots.keys() = ", refitPropTimePlots.plots.keys()
print "omegaPlots.plots.keys() = ", omegaPlots.plots.keys()

