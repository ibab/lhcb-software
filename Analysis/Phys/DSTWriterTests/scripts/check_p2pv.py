#!/usr/bin/env python
'''
Check some TES locations with Particles in a DST and print a summary of the number of related PVs and their location. Uses stored Particle->PV relations tables..
Useage:
./check_p2pv.py --input <some file name> --location <location of trunk - default "/Event"> --output <output file name>
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

class P2PVSummaryInfo(object) :
    def __init__(self, entries) :
        self.entries = entries
        self.p2pvmap = {}
    def addEntry(self, key, value) :
        addEntry(self.p2pvmap, key, value)

if __name__ == '__main__' :

    from GaudiConf.Configuration import *
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    import sys, getopt

    filename = ''
    location = '/Event'
    output = 'p2pv.txt'
    opts, args = getopt.getopt(sys.argv[1:], "l:i:o", ["input=", "location=", "output="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        elif o in ("-l", "--location") :
            location = a
        elif o in ('-o', '--output') :
            output = a
            
    assert(filename != '')

    outputFile = open(output, 'w')

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
        leaves = filter(lambda s : s.endswith('/Particles'), leaves)
        for leaf in leaves :
            particles = evtSvc[leaf]
            if particles :
                p2pvSummaries[leaf] = P2PVSummaryInfo(particles.size())
                p2pvloc = leaf.replace('/Particles',
                                       '/Particle2VertexRelations')
                p2pv = evtSvc[p2pvloc]
                if p2pv :
                    print 'Found Particle->PV', p2pv.relations().size(), 'relations in', p2pvloc
                    for p in particles :
                        rng = p2pv.relations(p)
                        if not rng.empty():
                            print 'Found a relation'
                            for rel in rng :
                                pv = rel.to()
                                pvLoc = pv.parent().registry().identifier()
                                p2pvSummaries[leaf].addEntry(pvLoc, 1)
                        else :
                            print 'Particle->PV relations empty'
    print '==================================================================='
    message = 'Analysed ' + str(nEvents) + ' in location ' + location
    outputFile.write(message+'\n')
    print message
    for particleloc, value in p2pvSummaries.iteritems() :
        if value.p2pvmap :
            separator = '-----------------------------------------------------------------'
            outputFile.write(separator+'\n')
            print separator
            
            message = particleloc + ' : ' +str(value.entries) + ' particles'
            outputFile.write(message+'\n')
            print message

            for loc, entries in value.p2pvmap.iteritems():
                message = '\t\t ' + loc + ' : ' +str( entries ) + ' related PVs'
                outputFile.write(message+'\n')
                print message

    outputFile.write(separator+'\n')
    print separator
    print 'Wrote summary to', output
    outputFile.close()
