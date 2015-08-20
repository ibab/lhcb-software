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

def analyseParticlesAndFlavTags(particles, flavTags) :
    if flavTags :
        for tag in flavTags :
            print 'Tagged B', tag.taggedB()
    else :
        print 'Found no FlavourTags'

        
if __name__ == '__main__' :

    _usage = """
    \t%prog filename [options]
    \tSearches in ROOT/BRANCH/Particles and ROOT/BRANCH/FlavourTags to get the FlavourTag corresponding to each Particle."""
    

    from DSTWriterTests.default_args import parser
    parser.set_defaults(output='flavtags.txt')
    parser.set_usage(_usage)
    (options, args) = parser.parse_args()
        
    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    locationRoot = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    branch  = options.branch                                    

    particleLoc = locationRoot + '/' + branch + '/Particles'
    flavTagLoc= locationRoot + '/' + branch + '/FlavourTags'

    # Configuration

    from Configurables import MessageSvc, LHCbApp
 
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
    appMgr.ExtSvc += ['DataOnDemandSvc' ]
    appMgr.initialize()

    evtSvc = appMgr.evtSvc()
    evtSel = appMgr.evtSel()
    tsvc= appMgr.toolsvc()

    from AnalysisPython import Functors
    nextEvent = Functors.NextEvent(appMgr, EvtMax=nevents)

    # open a DST or MicroDST
    evtSel.open(filename)

    nEvents=0

    while ( nextEvent() ) :
        nEvents+=1
#        print "Event ", nEvents, ". Getting data from ", particleLoc, p2lhcbidLoc
        analyseParticlesAndFlavTags(evtSvc[particleLoc], evtSvc[flavTagLoc])
