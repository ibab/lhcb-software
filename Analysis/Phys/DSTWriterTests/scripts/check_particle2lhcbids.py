#!/usr/bin/env python

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

def analyseParticlesAndLHCbIDs(particles, lhcbidmap) :
    if particles :
        for particle in particles :
            lhcbids = getLHCbIDs(particle, lhcbidmap)
            if lhcbids :
                print 'Found ', lhcbids.size(), 'LHCbIDs for particle!'
                for lhcbid in lhcbids :
                    print lhcbid.channelID()
            else :
                print 'Found no LHCbIDs for particle!'
    else :
        print 'Found no Particles'

        
if __name__ == '__main__' :

    _usage = """
    \t%prog filename [options]
    \tSearches in ROOT/BRANCH/Partilces and ROOT/BRANCH/Particle2LHCbIDMap to get the LHCbIDs corresponding to each particle."""

    from DSTWriterTests.default_args import parser
    parser.set_defaults(output='dst_contents.txt')
    parser.set_usage(_usage)
    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    locationRoot = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    branch = options.branch
    
    particleLoc = locationRoot + '/' + branch + '/Particles'
    p2lhcbidLoc= locationRoot + '/' + branch + '/Particle2LHCbIDMap'

    # Configuration

    from Configurables import MessageSvc
 
    MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
    
    from Gaudi.Configuration import EventSelector
    EventSelector().PrintFreq=100

    from Configurables import LHCbApp
    lhcbApp = LHCbApp()
    lhcbApp.DDDBtag = 'default'
    lhcbApp.CondDBtag = 'default'

    # GaudiPython

    from GaudiPython.Bindings import AppMgr
    import GaudiPython
    GaudiPython.loaddict('PhysDict')
    appMgr = AppMgr(outputlevel=3)
    appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.ExtSvc += ['DataOnDemandSvc' ]
    appMgr.initialize()

    evtSvc = appMgr.evtSvc()
    evtSel = appMgr.evtSel()
    tsvc= appMgr.toolsvc()

    from AnalysisPython import Functors
    nextEvent = Functors.NextEvent(appMgr, nevents)

    # open a DST or MicroDST
    evtSel.open(filename)

    nEvents=0

    while ( nextEvent() ) :
        nEvents+=1
        if verbose :
            print "Event ", nEvents, ". Getting data from ", particleLoc, p2lhcbidLoc
        analyseParticlesAndLHCbIDs(evtSvc[particleLoc], evtSvc[p2lhcbidLoc])
