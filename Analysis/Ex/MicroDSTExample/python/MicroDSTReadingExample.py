# Imports and typedefs
import sys, getopt
from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr
from math import sqrt, fabs
#==============================================================================
def nextEvent(am):
    am.run(1)
    return  am.evtSvc()['/Event'] != None
#==============================================================================
def deSmartRef(T) :
    if hasattr(T, 'target'): T = T.target()
    return T
#==============================================================================
def initMassPlots(pid, window):
    partProp=partSvc.findByStdHepID( pid )
    return TH1D(partProp.particle()+" mass",
                partProp.particle()+" mass" ,
                100,
                partProp.mass()-window,
                partProp.mass()+window)
#==============================================================================
def initMassResPlots(pid, window):
    partProp=partSvc.findByStdHepID( pid )    
    return TH1D(partProp.particle()+" Rec mass - MC mass",
                partProp.particle()+" Rec mass - MC mass" ,
                100,
                -window,
                +window)
#==============================================================================
def initProperTimePlots(pid, min, max):
    partProp=partSvc.findByStdHepID( pid )
    return TH1D(partProp.particle()+" proper time",
                partProp.particle()+" proper time" ,
                100,
                min,
                max)
#==============================================================================
def makeMassPlots(particle,plots):
    pid = particle.particleID().pid()
    if plots.has_key(pid) == False :
        plots[pid] = initMassPlots(pid, 500.)
    result = plots[pid].Fill(particle.momentum().mass())    
#==============================================================================
def makeMassResPlots(particle,mcParticle,plots):
    pid = particle.particleID().pid()
    offset = 10000
    if (pid < 0) : offset *= -1
    if plots.has_key(offset+pid) == False :
        plots[offset+pid] = initMassResPlots(pid, 500.)
    result = plots[offset+pid].Fill(particle.momentum().mass()-mcParticle.momentum().mass())    
#==============================================================================
def makeProperTimePlots(particle, vertex, fitter, plots):
    if (vertex != None) :
        pid = particle.particleID().pid()
        if ( particle.particleID().hasBottom() ):
            if plots.has_key(pid) == False :
                plots[pid] = initProperTimePlots(pid, -0.02, 0.04)
            time =Double(-100000.)
            error = Double(0.)
            chi2=Double(0.)
            fitter.fit(vertex, particle, time, error, chi2)
            result = plots[pid].Fill(time)    
#==============================================================================
def particleTreeLoop(particles, plots):
    for p in particles:
        p = deSmartRef(p)
        makeMassPlots(p, plots)
        table = evt[particle2mcPath]
        if (table != None) :
            assocRange = table.relations(p)
            if (assocRange.size() > 0) :
                assocMCParticle = assocRange[0].to()
                makeMassResPlots(p, assocMCParticle, plots)
        daughters = p.daughters()
        particleTreeLoop(daughters, plots)

#==============================================================================
def testFlavourTags(location, onegaPlot, catPlot) :
    tags = evt[location]
    if (tags!=None):
        for t in tags :
            print "Found B-tag\n", t
            print "For particle\n", t.taggedB()
            omegaPlot.Fill(t.omega())
            catPlot.Fill(t.category())
#==============================================================================
def countPVs(location, plot) :
    vertices = evt[location]
    nVerts=0
    if (vertices !=None) :
        print "Found ", vertices.size(), " PVs"
        nVerts = vertices.size()
    else :
        print "Found no PVs"

    plot.Fill(vertices.size())
    return nVerts
#==============================================================================
def trueDecayMassPlots(location, plots) :
    decays = evt['/Event/microDST/MC/Decays']
    if (decays != None):
        for mcp in decays: makeMassPlots(mcp, mcMassPlots)
        return 1
    else :
        return 0
#==============================================================================
def bestVertex(particle, table) :
    if (table !=None) :
        PVRange = bestVertexAssoc.relations(particle)
        if ( not PVRange.empty()) : return PVRange.back().to()
    else :
        return
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

sel = appMgr.evtSel()
evt = appMgr.evtSvc()
tools = appMgr.toolsvc()

appMgr.ExtSvc += ['ParticlePropertySvc']

partSvc = appMgr.partSvc()

properTimeFitter = tools.create('PropertimeFitter',
                                interface='ILifetimeFitter')

sel.open(microDSTFile)

mcMassPlots = {}
massPlots = {}
propTimePlots = {}
nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0

selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'
particle2mcPath = selectionPath + '/Particles/RelatedMCParticles'
vertexAssociationPath = selectionPath + '/Particle2VertexRelations'
mcParticlePath = '/Event/microDST/MC/Particles'
flavTagPath = selectionPath + "/Tagging"
pvLocation = "/Event/microDST/Rec/Vertex/Primary"
nPVPlot = TH1D( "# of PVs", "# of primary vertices", 5, -0.5, 4.5 )
omegaPlot = TH1D("Mis-tag fratcion", "Mis-tag fraction", 100, 0,1.0)
flavCatPlot = TH1D("Tagging category", "Tagging category", 11, -0.5,10.5)
while (nextEvent(appMgr)) :
#    evt.dump()
    nEvents+=1
    nPrimaryVertices += countPVs(pvLocation, nPVPlot)
    nMCEvents += trueDecayMassPlots('/Event/microDST/MC/Decays', mcMassPlots)
    particles = evt[particlePath]
    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        bestVertexAssoc = evt[vertexAssociationPath]
        particleTreeLoop(particles, massPlots)
        for p in particles:
            vertex = bestVertex(p, bestVertexAssoc)
            makeProperTimePlots(p,
                                vertex,
                                properTimeFitter,
                                propTimePlots)
        else :
            print "Found no particle->PV associations at ", vertexAssociationPath
    testFlavourTags(flavTagPath, omegaPlot, flavCatPlot)
    
print "Found MC info in ", nMCEvents, "/", nEvents, " events"
print "Found Reco info in ", nRecEvents, "/", nEvents, "events"
print "Found ", nPrimaryVertices, " PVs in ", nEvents, "events"
print "Found ", nParticles, " B candidates in ", nEvents, "events"

print "massPlots.keys() = ", massPlots.keys()

#massPlots[511].Draw()
#mcMassPlots[511].SetLineColor(2)
#mcMassPlots[511].Draw('same')
#propTimePlots[531].Draw()

