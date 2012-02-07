#!/usr/bin/env python
'''
Check a Particle location in a DST and look for ProtoParticles.
Useage:
./check_protoparticles.py --help
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

if __name__ == '__main__' :

    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    _usage = """
    \t%prog filename [options]
    \tGets Particles from ROOT/BRANCH/Partilces and follows their ProtoParticle SmartRefs."""

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
    particleLoc = particleLoc.replace('//', '/')
    particleLocation = particleLoc.replace('/Particles/Particles', '/Particles')

    from Configurables import DaVinci
    dv = DaVinci()
    dv.DataType = '2010'
    dv.Simulation = False
    dv.InputType = 'DST'
    appMgr = AppMgr(outputlevel=4)
    appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.initialize()

    import atexit
    atexit.register(appMgr.exit)

    evtSvc = appMgr.evtSvc()

    evtSel = appMgr.evtSel()

    nextEvent = Functors.NextEvent(appMgr, nevents)

    evtSel.open(filename)

    nEvents=0
    nParticles = 0
    nProtoParticles = 0


    while ( nextEvent() ) :
        nEvents+=1
        particles = evtSvc[particleLocation]
        if particles :
            nParticles += particles.size()
            for p in particles :
                protoParticle = p.proto()
                if protoParticle :
                    nProtoParticles += 1
                    print protoParticle
                    print protoParticle.track()
    if verbose :
        print '======================================================================================'
        print 'Checked location', particleLocation
        print 'Found', nParticles, 'Particles and', nProtoParticles, 'ProtoParticles in', nEvents, 'events'
        print '======================================================================================'
