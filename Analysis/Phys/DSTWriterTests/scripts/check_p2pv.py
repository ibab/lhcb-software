#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --root <location of trunk - defailt "/Event"> --location <location of selection on top of root>
'''
from __future__ import division

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

from AnalysisPython.Helpers import deSmartRef

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

def particleLoop(particles, evtSvc, p2pvmap) :
    if particles :
        p2pv = evtSvc[location+'/Particle2VertexRelations']
        for p in particles :
            p = deSmartRef(p)
            loc =  p.parent().registry().identifier()
            nRels = 0
            if p2pv :
                rng = p2pv.relations(p)
                nRels = rng.size()
            addEntry(p2pvmap,loc, nRels)
            particleLoop(p.daughters(), evtSvc, p2pvmap)



if __name__ == '__main__' :

    from GaudiConf.Configuration import *
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    import sys, getopt

    filename = ''
    root = '/Event'
    opts, args = getopt.getopt(sys.argv[1:], "l:i:r", ["input=", "location=", "root="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        if o in ("-l", "--location") :
            location = a
        if o in ("-r", "--root") :
            root = a

    location = root + '/' + location
            
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
    nCandidates = {}
    p2pvmap = {}
    nMoreThanOnePV = 0
    particleLocation = location+'/Particles'
    while ( nextEvent() ) :
        nEvents+=1
        particles = evtSvc[particleLocation]
        if particles :
            particleLoop(particles, evtSvc, p2pvmap)
            pvs = evtSvc[root+'/Rec/Vertex/Primary']
            if pvs and pvs.size() > 1:
                nMoreThanOnePV += 1
            for loc in p2pvmap.keys() :
                addEntry(nCandidates, loc, evtSvc[loc].size()) 
    length = len(sorted(p2pvmap.keys(), cmp = lambda x,y : cmp(len(y), len(x)))[0])+2
    print '----------------------------------------------------------------------------------'
    print "Analysed ", nEvents, " events"
#    print "Found", nCandidates, "candidates in", particleLocation
    print "Found", nMoreThanOnePV, "events with more than one PV in events with candidates"
    for loc in p2pvmap.keys() :
        message = loc.ljust(length) + str('Particle->PV relations/Candidates: '+ str(p2pvmap[loc])+'/'+ str(nCandidates[loc])).rjust(10)
        print message
    print '----------------------------------------------------------------------------------'

 
