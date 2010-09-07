#!/usr/bin/env python
'''
Check standard TES locations with Particle->PV relations in a DST and print a summary of the number relations for each From, To TES location pair. Uses stored Particle->PV relations tables.
Useage:
./check_p2pvtable.py --input <some file name> --location <location of trunk - default "/Event"> --table-name <Name of relations table. Default: Particle2VertexRelations> --output <output file name>
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
    tablename = 'Particle2VertexRelations'
    output = 'relations.txt'
    opts, args = getopt.getopt(sys.argv[1:], "l:i:t:o", ["input=",
                                                       "location=",
                                                       "table-name=",
                                                       "output="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        if o in ("-l", "--location") :
            location = a
        if o in ("-t", "--table-name") :
            tablename = a
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
        leaves = filter(lambda s : s.endswith('/'+ tablename), leaves)
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
    message = 'Analysed ' + str(nEvents) + ' events in location ' + location
    outputFile.write(message+'\n')
    print message
    for key, value in p2pvSummaries.iteritems() :
        separator = '-----------------------------------------------------------------'
        outputFile.write(separator+'\n')
        print separator
        message = str( key ) + ' : ' + str( value ) + ' entries'
        outputFile.write(message+'\n')
        print message
        outputFile.write(separator+'\n')
        print separator

    outputFile.write(separator+'\n')
    print separator
    print 'Wrote summary to', output
    outputFile.close()
