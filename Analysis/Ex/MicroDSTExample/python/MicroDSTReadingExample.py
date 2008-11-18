# Imports and typedefs
import sys, getopt
from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr
from math import sqrt, fabs
from GaudiKernel import SystemOfUnits, PhysicalConstants
from LHCbMath import XYZVector, XYZPoint
#==============================================================================
def nextEvent(am):
    am.run(1)
    return  am.evtSvc()['/Event'] != None
#==============================================================================
def deSmartRef(T) :
    if hasattr(T, 'target'): T = T.target()
    return T
#==============================================================================
def decayVertex(particle) :
    vertices = particle.endVertices()
    iCount=0
    for v in vertices :
        return deSmartRef(v)
#==============================================================================
def properTime(particle, vertex, fitter) :
    tau =Double(-100000.)
    if (vertex != None) :
        pid = particle.particleID().pid()
        error = Double(0.)
        chi2=Double(0.)
        fitter.fit(vertex, particle, tau, error, chi2)
    return tau/SystemOfUnits.picosecond
#==============================================================================
def properTimeMC(particle) :
    tau = -99999.
    if (particle.particleID().pid() == -99000000 ) : return tau
    originVertex = particle.originVertex()
    decayVtx = decayVertex(particle)
    if (decayVtx !=None) :
        decayPos = decayVtx.position()
        originPos = originVertex.position()
        flightVector = XYZVector( decayPos.x()-originPos.x(),
                                  decayPos.y()-originPos.y(),
                                  decayPos.z()-originPos.z() )
        p = particle.momentum()
        m = p.mass()
        tau = ( (m/(SystemOfUnits.picosecond)) * (p.Vect().Dot(flightVector)) ) / p.Vect().mag2()
        tau /= PhysicalConstants.c_light
    return tau
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
def makeMCProperTimePlots(particle,plots):
    pid = particle.particleID().pid()
    if ( particle.particleID().hasBottom() ):
        if plots.has_key(pid) == False :
            plots[pid] = initProperTimePlots(pid, -2., 5.0)
        tau = properTimeMC(particle)
        result = plots[pid].Fill(tau)
#==============================================================================
def makeMassResPlots(particle,mcParticle,plots):
    pid = particle.particleID().pid()
    offset = 10000
    if (pid < 0) : offset *= -1
    if plots.has_key(offset+pid) == False :
        plots[offset+pid] = initMassResPlots(pid, 500.)
    result = plots[offset+pid].Fill(particle.momentum().mass()-mcParticle.momentum().mass())    
#==============================================================================
def makeProperTimePlots(particle, tau, plots):
    pid = particle.particleID().pid()
    if ( particle.particleID().hasBottom() ):
        if plots.has_key(pid) == False :
            plots[pid] = initProperTimePlots(pid, -2., 5.0)
        result = plots[pid].Fill(tau)
#==============================================================================
def assocMCP(particle, table) :
    if (table != None) :
        assocRange = table.relations(particle)
        if (assocRange.size() > 0) :
            return assocRange[0].to()
    else :
        print "No table found!"
#==============================================================================
def particleTreeLoop(particles, plots, table):
    for p in particles:
        p = deSmartRef(p)
        makeMassPlots(p, plots)
        assocMCParticle = assocMCP(p, table)
        if (assocMCParticle !=None) :
            makeMassResPlots(p, assocMCParticle, plots)
        daughters = p.daughters()
        particleTreeLoop(daughters, plots, table)

#==============================================================================
def testFlavourTags(location, onegaPlot, catPlot) :
    tags = evt[location]
    if (tags!=None):
        for t in tags :
#            print "Found B-tag\n", t
#            print "For particle\n", t.taggedB()
            omegaPlot.Fill(t.omega())
            catPlot.Fill(t.category())
#==============================================================================
def countPVs(location, plot) :
    vertices = evt[location]
    nVerts=0
    if (vertices !=None) :
        nVerts = vertices.size()
    else :
        print "Found no PVs"

    plot.Fill(nVerts)
    return nVerts
#==============================================================================
def MCDecayPlots(location, massPlots, propTimePlots) :
    decays = evt[location]
    if (decays != None):
        for mcp in decays:
            makeMassPlots(mcp, massPlots)
            makeMCProperTimePlots(mcp, propTimePlots)
        return 1
    else :
        return 0
#==============================================================================
def bestVertex(particle, table) :
    if (table !=None) :
        PVRange = table.relations(particle)
        if ( not PVRange.empty()) : return PVRange.back().to()
    else :
        print "Found no particle->PV associations"
        return
#==============================================================================
def getEvtStuff(location) :
    stuff = evt[location]
    if (stuff ==None):
        print "Found no stuff in ", location
    return stuff
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

'''
Gaudi::XYZVector TMP2 = (*ivert)->position() - MC->originVertex()->position();
double m_mctau = ((1/(picosecond*c_light))  * (MC->momentum().mass())*
                  (MC->momentum().Vect()).Dot(TMP2))/ MC->momentum().Vect().mag2();
'''
sel.open(microDSTFile)

mcMassPlots = {}
mcPropTimePlots = {}
massPlots = {}
propTimePlots = {}
refitPropTimePlots = {}
nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0

selectionPath = locationRoot +  '/Phys/' + selection
particlePath = selectionPath + '/Particles'
particle2mcPath = selectionPath + '/Particles/RelatedMCParticles'
stdVertexAssocPath = selectionPath + '/Particle2VertexRelations'
refitVertexAssocPath = selectionPath + '/Particle2ReFittedVertexRelations'
mcParticlePath = '/Event/microDST/MC/Particles'
flavTagPath = selectionPath + "/Tagging"
pvLocation = "/Event/microDST/Rec/Vertex/Primary"
nPVPlot = TH1D( "# of PVs", "# of primary vertices", 5, -0.5, 4.5 )
omegaPlot = TH1D("Mis-tag fratcion", "Mis-tag fraction", 100, 0,1.0)
flavCatPlot = TH1D("Tagging category", "Tagging category", 11, -0.5,10.5)
stdPropTimeResPlot = TH1D("tau-tau_MC", "tau-tau_MC", 100, -0.2,0.5)
refitPropTimeResPlot = TH1D("tau-tau_MC", "tau-tau_MC", 100, -0.2,0.5)
while (nextEvent(appMgr)) :
#    evt.dump()
    nEvents+=1
    nPrimaryVertices += countPVs(pvLocation, nPVPlot)
#    nMCEvents += MCDecayPlots('/Event/microDST/MC/Decays', mcMassPlots)
    nMCEvents += MCDecayPlots(mcParticlePath, mcMassPlots, mcPropTimePlots)
    particles = evt[particlePath]
    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        stdBestVertexAssoc = getEvtStuff(stdVertexAssocPath)
        refitBestVertexAssoc = getEvtStuff(refitVertexAssocPath)
        p2MCPTable = evt[particle2mcPath]
        particleTreeLoop(particles, massPlots, p2MCPTable)
        for p in particles:
            stdVertex = bestVertex(p, stdBestVertexAssoc)
            stdPropTime = properTime(p, stdVertex, properTimeFitter)
            makeProperTimePlots(p,
                                stdPropTime,
                                propTimePlots)
            refitVertex = bestVertex(p, refitBestVertexAssoc)
            refitPropTime = properTime(p, refitVertex, properTimeFitter)
            makeProperTimePlots(p,
                                refitPropTime,
                                refitPropTimePlots)
            assocMCPart = (assocMCP(p, p2MCPTable))
            if (assocMCPart != None) :
                MCPropTime = properTimeMC(assocMCP(p, p2MCPTable))
                stdPropTimeRes = stdPropTime-MCPropTime
                stdPropTimeResPlot.Fill(stdPropTimeRes)
                refitProtTimeRes = refitPropTime-MCPropTime
                refitPropTimeResPlot.Fill(refitProtTimeRes)
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

