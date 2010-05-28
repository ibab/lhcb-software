#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <location of trunk - defailt "/Event">
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

if __name__ == '__main__' :

    from GaudiConf.Configuration import *
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    import sys, getopt

    filename = ''
    location = '/Event'
    opts, args = getopt.getopt(sys.argv[1:], "l:i", ["input=", "location="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        if o in ("-l", "--location") :
            location = a
            
    assert(filename != '')

    lhcbApp = LHCbApp(DDDBtag = 'default',
                      CondDBtag = 'default')

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
    nSelEvents = {}
    nObjects = {}
    
    while ( nextEvent() ) :
        nEvents+=1
        particles = evtSvc[location+'/Particles']
        if particles :
            print 'Found', particles.size(), 'particles in', location
            p2pv = evtSvc[location+'/Particle2VertexRelations']
            if not p2pv :
                print 'Found no P2PV!!!'
            else :
                for p in particles :
                    rng = p2pv.relations(p)
                    print 'Found', rng.size(), 'P2PV relations'
