#!/usr/bin/env python
'''
Check a Particle location in a DST and look for ProtoParticles.
Useage:
./check_protoparticles.py --input <some file name> --location <TES location of Particles>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

if __name__ == '__main__' :

    import sys, getopt
    from GaudiConf.Configuration import *
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

#    particleLocation = '/Event/Dimuon/Phys/StdVeryLooseMuons/Particles'
    particleLocation = ''
    filename = ''

    opts, args = getopt.getopt(sys.argv[1:], "l:i", ["input=", "location="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        if o in ("-l", "--location") :
            particleLocation = a
            
    assert(filename != '')
    assert(particleLocation != '')
#    lhcbApp = LHCbApp(DDDBtag = 'default',
#                      CondDBtag = 'default')

    from Configurables import DaVinci
    dv = DaVinci()
    dv.DataType = 'MC09'
    dv.Simulation = True
    dv.InputType = 'DST'
    appMgr = AppMgr(outputlevel=4)
    appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.initialize()

    import atexit
    atexit.register(appMgr.exit)

    evtSvc = appMgr.evtSvc()

    evtSel = appMgr.evtSel()

    nextEvent = Functors.NextEvent(appMgr)

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

    print '======================================================================================'
    print 'Checked location', particleLocation
    print 'Found', nParticles, 'Particles and', nProtoParticles, 'ProtoParticles in', nEvents, 'events'
    print '======================================================================================'
