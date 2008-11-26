# Imports and typedefs
import sys, getopt
from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr
from math import sqrt, fabs
from GaudiKernel import SystemOfUnits, PhysicalConstants
from LHCbMath import XYZVector, XYZPoint
from MicroDSTExample import Helpers, Functors
GenPlotter = Functors.GenericPlotter
#==============================================================================
def particleTreeLoop(particles, plotter):
    for p in particles:
        p = Helpers.deSmartRef(p)
        plotter(p)
        daughters = p.daughters()
        particleTreeLoop(daughters, plotter)
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
microDSTFile = '/afs/cern.ch/lhcb/group/davinci/vol1/DEV/data/DV_v20r2/testBs2JpsiPhi_5Kevt_pythonConfig.dst'

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


appMgr = AppMgr(outputlevel=4)
appMgr.config( files = ['$STDOPTS/LHCbApplication.opts',
                        '$STDOPTS/SimDicts.opts',
                        '$DDDBROOT/options/DC06.opts'])
appMgr.initialize()

appMgr.ExtSvc += ['ParticlePropertySvc']

eventLoop = Helpers.EventLoop(appMgr)

properTimeFitter = eventLoop.tools.create('PropertimeFitter',
                                          interface='ILifetimeFitter')

eventLoop.sel.open(microDSTFile)

mcMassPlots = Helpers.Plots("MC mass")
mcPropTimePlots = Helpers.Plots("MC proper time")
massPlots = Helpers.Plots("Mass")
massResPlots = Helpers.Plots("Mass resolution")
propTimePlots = Helpers.Plots("Proper time")
refitPropTimePlots = Helpers.Plots("Re-fitted proper time")
propTimeResPlots = Helpers.Plots("Proper-time resolution")
omegaPlots = Helpers.Plots("Mis-tag fratcion")
flavCatPlots = Helpers.Plots("Tagging category")
nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0

selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'
particle2mcPath = selectionPath + '/Particles/RelatedMCParticles'
stdVertexAssocPath = selectionPath + '/Particle2VertexRelations'
refitVertexPath = selectionPath + '/RefittedVertices'
refitVertexAssocPath = selectionPath + '/Particle2ReFittedVertexRelations'
mcParticlePath = '/Event/microDST/MC/Particles'
flavTagPath = selectionPath + "/Tagging"
pvLocation = "/Event/microDST/Rec/Vertex/Primary"
nPVPlot = TH1D( "# of PVs", "# of primary vertices", 5, -0.5, 4.5 )

stdPropTimeResPlot = TH1D("tau-tau_MC", "tau-tau_MC", 100, -0.2,0.5)
refitPropTimeResPlot = TH1D("tau-tau_MC", "tau-tau_MC", 100, -0.2,0.5)
refitVertexZ =  TH1D("re-fit PVz", "re-fit PVz", 50, -100.,100.)
vertexZ =  TH1D("PVz", "PVz", 50, -100.,100.)

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

while (eventLoop.nextEvent() ) :
#    evt.dump()
    nEvents+=1
    nPrimaryVertices += eventLoop.countAndPlotNEntries(pvLocation, nPVPlot)
    mcParts = eventLoop.getEvtStuff(mcParticlePath)
    
    nMCEvents += EventDataPlots(mcParts, mcMassPlotter)
    EventDataPlots(mcParts, mcPropTimePlotter)
    particles = eventLoop.getEvtStuff(particlePath)
    flavTags = eventLoop.getEvtStuff(flavTagPath)
    EventDataPlots(flavTags, tagOmegaPlotter)

    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        stdBestVertexAssoc = eventLoop.getEvtStuff(stdVertexAssocPath)
        refitBestVertexAssoc = eventLoop.getEvtStuff(refitVertexAssocPath)
        p2MCPTable = eventLoop.getEvtStuff(particle2mcPath)
        massResFunc = Functors.MassRes(p2MCPTable)
        particleMassResPlotter = GenPlotter(massResPlots,
                                            massResFunc,
                                            Helpers.pid,
                                            particleNameFunc,
                                            Helpers.HistoAttributes(100,-100., 100.),
                                            "rec mass - MC mass")
        particleTreeLoop(particles, particleMassPlotter)
        particleTreeLoop(particles, particleMassResPlotter)

        stdTauFunc = Functors.PropTime(Functors.BestVertex(stdBestVertexAssoc),
                                       properTimeFitter)
        tauPlotter = GenPlotter(propTimePlots,
                                stdTauFunc,
                                Helpers.pid,
                                particleNameFunc,
                                tauHistoAtts,
                                "proper time")

        reFitPVTauFunc = Functors.PropTime(Functors.BestVertex(refitBestVertexAssoc),
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
            stdVertex = Helpers.bestVertex(p, stdBestVertexAssoc)
            stdPropTime = Helpers.properTime(p, stdVertex, properTimeFitter)
            refitVertex = Helpers.bestVertex(p, refitBestVertexAssoc)
            pMom = p.momentum()
            refitVertices = eventLoop.getEvtStuff(refitVertexPath)
            for rv in refitVertices :
                refitVertexZ.Fill(rv.position().z())
            if (refitVertex != None) :
                refitPropTime = Helpers.properTime(p, refitVertex, properTimeFitter)
            assocMCPart = (Helpers.assocMCP(p, p2MCPTable))
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

print "massPlots.keys() = ", massPlots.plots.keys()

#massPlots[511].Draw()
#mcMassPlots[511].SetLineColor(2)
#mcMassPlots[511].Draw('same')
#propTimePlots[531].Draw()

