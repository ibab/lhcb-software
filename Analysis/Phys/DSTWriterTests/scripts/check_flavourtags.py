#!/usr/bin/env python
import sys, getopt
#==============================================================================
def printHelp():
    print "Usage: python -i MicroDSTReadingExample [options]"
    print "Options:\n"
    print "\t--input         Input MicroDST file"
    print "                  Default ''"
    print "                  Default 'DC06selBs2JpsiPhi_unbiased'"
    print "\t--root          TES root of everything."
    print "                  Default 'Event/MultiDiMuon'"


def analyseParticle(particle) :
    if particle :
        print particle
        analyseProto(particle.proto())
    else :
        print 'Found no Particle'

def getLHCbIDs(particle, lhcbidmap) :
    return lhcbidmap(particle)
#    for p in lhcbidmap.keys() :
#        if p.target() == particle :
#            return lhcbidmap(p)

def analyseParticlesAndFlavTags(particles, flavTags) :
    if flavTags :
        for tag in flavTags :
            print 'Tagged B', tag.taggedB()
    else :
        print 'Found no FlavourTags'

        
if __name__ == '__main__' :

    locationRoot = '/Event/Bu2JpsiK'

    microDSTFile = ['']

    opts, args = getopt.getopt(sys.argv[1:], "i:r:h", ["input=", "root=", "help"])

    for o, a in opts:
        if o in ("-h", "--help"):
            printHelp()
        elif o in ("-i", "--input"):
            microDSTFile=a
        elif o in ("-r", "--root"):
            locationRoot = a

#    particleLoc = locationRoot + '/Dimuon/Phys/StdLooseMuons/Particles'
    particleLoc = locationRoot + '/Dimuon/Phys/Bu2JpsiKLine/Particles'
    flavTagLoc= locationRoot + '/Dimuon/Phys/Bu2JpsiKLine/FlavourTags'
#    pvLoc       = locationRoot +  '/Rec/Vertex/Primary'
#    pvLoc       = locationRoot +  '/Phys/SelBs2Jpsi2MuMuPhi2KK/ReFitPVs_SeqBs2Jpsi2MuMuPhi2KK_PVs'
    # Configuration

    from GaudiConf.Configuration import *

    from Configurables import MessageSvc
 
    MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
    
    from Gaudi.Configuration import EventSelector
    EventSelector().PrintFreq=100

    lhcbApp = LHCbApp()
    lhcbApp.DDDBtag = 'default'
    lhcbApp.CondDBtag = 'default'

    # GaudiPython

    from GaudiPython.Bindings import AppMgr
    import GaudiPython
    GaudiPython.loaddict('PhysDict')
    appMgr = AppMgr(outputlevel=3)
    appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.ExtSvc += ['DataOnDemandSvc', 'ANNDispatchSvc']
    appMgr.initialize()

    evtSvc = appMgr.evtSvc()
    evtSel = appMgr.evtSel()
    tsvc= appMgr.toolsvc()

    from AnalysisPython import Functors
    nextEvent = Functors.NextEvent(appMgr, EvtMax=10)

    # open a DST or MicroDST
    evtSel.open(microDSTFile)

    nEvents=0

    while ( nextEvent() ) :
        nEvents+=1
#        print "Event ", nEvents, ". Getting data from ", particleLoc, p2lhcbidLoc
        analyseParticlesAndFlavTags(evtSvc[particleLoc], evtSvc[flavTagLoc])
