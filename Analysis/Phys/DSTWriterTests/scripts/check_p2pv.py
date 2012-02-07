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

    from Configurables import LHCbApp
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    from DSTWriterTests.default_args import parser
    parser.add_option('--p2pvmap', type='string', dest='p2pvmap', default='Particle2VertexRelations', help='Name Particle -> PV relations table containers [default: %default]')
    parser.remove_option('--branch')
    parser.set_defaults(output='p2pv.txt')

    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    location = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    p2pvmapname = '/' + options.p2pvmap
    p2pvmapname = p2pvmapname.replace('//', '/')
    
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

    nextEvent = Functors.NextEvent(appMgr, nevents)

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
                                       p2pvmapname)
                p2pv = evtSvc[p2pvloc]
                if p2pv :
                    if verbose :
                        print 'Found Particle->PV', p2pv.relations().size(), 'relations in', p2pvloc
                    for p in particles :
                        rng = p2pv.relations(p)
                        if not rng.empty():
                            if verbose :
                                print 'Found a relation'
                            for rel in rng :
                                pv = rel.to()
                                pvLoc = pv.parent().registry().identifier()
                                p2pvSummaries[leaf].addEntry(pvLoc, 1)
                        else :
                            if verbose :
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
