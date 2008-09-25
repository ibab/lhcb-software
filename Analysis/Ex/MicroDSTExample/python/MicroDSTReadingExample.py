# Imports and typedefs
from ROOT import TCanvas, TH1D, Double
from GaudiPython import gbl, AppMgr
from math import sqrt, fabs
#from units import GeV
from LinkerInstances.eventassoc import *


std = gbl.std
LHCb = gbl.LHCb

sc = gbl.StatusCode
locationRoot = '/Event/microDST/Phys/'

def nextEvent(am):
    am.run(1)
    return  am.evtSvc()['/Event/microDST/Rec/Header'] != None

def deSmartRef(T) :
    if hasattr(T, 'target'): T = T.target()
    return T

def initMassPlots(pid, window):
    partProp=partSvc.findByStdHepID( pid )
    return TH1D(partProp.particle()+" mass",
                partProp.particle()+" mass" ,
                100,
                partProp.mass()-window,
                partProp.mass()+window)

def initMassResPlots(pid, window):
    partProp=partSvc.findByStdHepID( pid )    
    return TH1D(partProp.particle()+" Rec mass - MC mass",
                partProp.particle()+" Rec mass - MC mass" ,
                100,
                -window,
                +window)

def initProperTimePlots(pid, min, max):
    partProp=partSvc.findByStdHepID( pid )
    return TH1D(partProp.particle()+" proper time",
                partProp.particle()+" proper time" ,
                100,
                min,
                max)

def makeMassPlots(particle,plots):
    pid = particle.particleID().pid()
    if plots.has_key(pid) == False :
        plots[pid] = initMassPlots(pid, 500.)
    result = plots[pid].Fill(particle.momentum().mass())    

def makeMassResPlots(particle,mcParticle,plots):
    pid = particle.particleID().pid()
    offset = 10000
    if (pid < 0) : offset *= -1
    if plots.has_key(offset+pid) == False :
        plots[offset+pid] = initMassResPlots(pid, 500.)
    result = plots[offset+pid].Fill(particle.momentum().mass()-mcParticle.momentum().mass())    

def makeProperTimePlots(particle, vertex, fitter, plots):
    pid = particle.particleID().pid()
    if ( particle.particleID().hasBottom() ):
        if plots.has_key(pid) == False :
            plots[pid] = initProperTimePlots(pid, -0.02, 0.04)
        time =Double(-100000.)
        error = Double(0.)
        chi2=Double(0.)
        fitter.fit(vertex, particle, time, error, chi2)
        result = plots[pid].Fill(time)    

def particleLoop(particles, plots):
    for p in particles:
        p = deSmartRef(p)
        makeMassPlots(p, plots)
        table = evt[particle2mcPath]
        if (table != None) :
            assocRange = table.relations(p)
            if (assocRange.size() > 0) :
                assocMCParticle = assocRange[0].to()
                makeMassResPlots(p, assocMCParticle, plots)
        else :
            print "Did NOT FIND Particle->MCParticle Relations table!!!!"

        daughters = p.daughters()
        particleLoop(daughters, plots)



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

#sel.open('$HOME/cmtDaVinci/DaVinci_v20r2/Phys/DaVinci/v20r2/cmt/testBs2JpsiPhi_5Kevt.dst')
sel.open('../options/MicroDST.dst')
#sel.open('../options/testBs2JpsiPhi_5Kevt.dst')
mcMassPlots = {}
massPlots = {}
propTimePlots = {}
nEvents=0
nMCEvents=0
nRecEvents=0
nParticles = 0
nPrimaryVertices = 0
basePath = '/Event/microDST/Phys/DC06selBs2JpsiPhi_unbiased'
#basePath = '/Event/microDST/Phys/SelB2HH'
particlePath = basePath + '/Particles'
particle2mcPath = basePath + '/Particles/RelatedMCParticles'
vertexAssociationPath = basePath + '/Particle2VertexRelations'
mcParticlePath = '/Event/microDST/MC/Particles'
while (nextEvent(appMgr)) :
#    print "HELLO!!!!!"
#    evt.dump()
    nEvents+=1
    vertices = evt['/Event/microDST/Rec/Vertex/Primary']
    if (vertices !=None) :
        nPrimaryVertices += 1
    decays = evt['/Event/microDST/MC/Decays']
    if (decays != None):
        for mcp in decays: makeMassPlots(mcp, mcMassPlots)
        nMCEvents += 1
    particles = evt[particlePath]
    if (particles!=None):
        nRecEvents+=1
        nParticles += particles.size()
        bestVertexAssoc = evt[vertexAssociationPath]
        particleLoop(particles, massPlots)
        for p in particles:
            if (bestVertexAssoc !=None) :
                PVRange = bestVertexAssoc.relations(p)
                vertex = PVRange.back().to()
                makeProperTimePlots(p,
                                    vertex,
                                    properTimeFitter,
                                    propTimePlots)
            else :
                print "Found no particle->PV associations at ", vertexAssociationPath

            
print "Found MC info in ", nMCEvents, "/", nEvents, " events"
print "Found Reco info in ", nRecEvents, "/", nEvents, "events"
print "Found ", nPrimaryVertices, " PVs in ", nEvents, "events"
print "Found ", nParticles, " B candidates in ", nEvents, "events"

#massPlots[511].Draw()
#mcMassPlots[511].SetLineColor(2)
#mcMassPlots[511].Draw('same')
#propTimePlots[531].Draw()

