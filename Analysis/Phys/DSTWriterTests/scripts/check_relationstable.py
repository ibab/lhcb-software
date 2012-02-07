#!/usr/bin/env python
'''
Check standard TES locations with Particle->PV relations in a DST and print a summary of the number relations for each From, To TES location pair. Uses stored Particle->PV relations tables.
Useage:
./check_p2pvtable.py --help
'''

__author__ = "Juan PALACIOS juan.palacios@cern.ch"

def addEntry(self, key, value) :
    if self.has_key(key) :
        self[key] += value
    else :
        self[key] = value

if __name__ == '__main__' :

    from Configurables import LHCbApp
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    from DSTWriterTests.default_args import parser
    parser.set_defaults(output='relations.txt')

    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    location = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    tablename  = options.branch                                        

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
        leaves = filter(lambda s : s.endswith('/'+ tablename), leaves)
        for leaf in leaves :
            p2pv = evtSvc[leaf]
            if p2pv :
                rels = p2pv.relations()
                if not rels.empty() :
                    if verbose :
                        print 'Found Particle->PV', rels.size(), 'relations in', leaf
                    for r in rels :
                        pv = r.to()
                        p = r._from()
                        toLoc = str(pv)
                        if hasattr(pv, 'parent') :
                            toLoc = pv.parent().registry().identifier()
                        fromLoc = p.parent().registry().identifier()
                        addEntry(p2pvSummaries, (fromLoc, toLoc), 1 )
                else :
                    if verbose :
                        print 'Particle->PV relations', leaf, 'empty'

    _printMessage = '===================================================================\n'
    message = 'Analysed ' + str(nEvents) + ' events in location ' + location + ', ' + tablename +'\n'
    _printMessage += message
    outputFile.write(message)
    _printMessage += message
    for key, value in p2pvSummaries.iteritems() :
        separator = '-----------------------------------------------------------------\n'
        outputFile.write(separator)
        _printMessage += separator
        message = str( key ) + ' : ' + str( value ) + ' entries'
        outputFile.write(message+'\n')
        _printMessage += message
        outputFile.write(separator+'\n')
        _printMessage += separator

    outputFile.write(separator+'\n')

    _printMessage += separator
    _printMessage += 'Wrote summary to' + output

    if verbose :
        print _printMessage
        
    outputFile.close()
