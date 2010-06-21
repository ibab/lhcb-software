#!/usr/bin/env python
'''
Check standard TES locations with Particle->PV relations in a DST and print a summary of the number relations for each From, To TES location pair. Uses stored Particle->PV relations tables.
Useage:
./check_p2pvtable.py --input <some file name> --location <location of trunk - default "/Event">
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
    p2pvMap = {}
    p2pvSummaries = {}
    
    while ( nextEvent() ) :
        nEvents+=1
        leaves = evtSvc.nodes(node = location, forceload = True)
        leaves = filter(lambda s : s.endswith('/Particle2VertexRelations'), leaves)
        for leaf in leaves :
            p2pv = evtSvc[leaf]
            if p2pv :
                rels = p2pv.relations()
                if not rels.empty() :
                    print 'Found Particle->PV', rels.size(), 'relations in', leaf
                    for r in rels :
                        pv = r.to()
                        p = r._from()
                        pvLoc = pv.parent().registry().identifier()
                        pLoc = p.parent().registry().identifier()
                        addEntry(p2pvSummaries, (pLoc, pvLoc), 1 )
                else :
                    print 'Particle->PV relations', leaf, 'empty'

    print '==================================================================='
    print 'Analysed', nEvents, 'in location', location
    for key, value in p2pvSummaries.iteritems() :
        print '-----------------------------------------------------------------'
        print key, ':', value, 'entries'
    print '-----------------------------------------------------------------'
