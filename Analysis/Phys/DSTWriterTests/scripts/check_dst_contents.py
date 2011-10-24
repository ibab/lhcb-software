#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py <dst name> [options]
./check_dst_contents.py --help
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
    from GaudiConf import IOHelper

    from DSTWriterTests.default_args import parser
    parser.remove_option('--branch')
    parser.set_defaults(output='dst_contents.txt')

    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    locationRoot = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    
    outputFile = open(output, 'w')

    lhcbApp = LHCbApp(DDDBtag = 'default',
                      CondDBtag = 'default')

    IOHelper().inputFiles([filename])

    appMgr = AppMgr(outputlevel=4)
    #appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.initialize()

    import atexit
    atexit.register(appMgr.exit)

    evtSvc = appMgr.evtSvc()

    evtSel = appMgr.evtSel()

    nextEvent = Functors.NextEvent(appMgr, nevents)

    #evtSel.open(filename)

    nEvents=0
    nSelEvents = {}
    nObjects = {}
    
    while ( nextEvent() ) :
        nEvents+=1
        nodes = evtSvc.nodes(node = locationRoot, forceload=True)
        for loc in nodes :
            data = evtSvc[loc]
            if data :
                addEntry(nSelEvents, loc, 1)
                if hasattr(data, 'size') :
                    addEntry(nObjects, loc, data.size())
                else :
                    addEntry(nObjects, loc, 1)

    nObjects.keys().sort()
    length = len(sorted(nObjects.keys(), cmp = lambda x,y : cmp(len(y), len(x)))[0])+2

    _printMessage = ''

    for loc in nObjects.keys() :
        if nObjects[loc] > 0 :
            message = loc.ljust(length) + str('Events: '+ str(nSelEvents[loc])+'. Objects: '+ str(nObjects[loc])).rjust(10) + '\n'
            outputFile.write(message)
            _printMessage += message
    outputFile.write("Analysed " + str(nEvents) + " events")
    if verbose :
        print '----------------------------------------------------------------------------------'
        print "Analysed", nEvents, "events"
        print _printMessage
        print '----------------------------------------------------------------------------------'
        print 'Wrote summary to', output
    outputFile.close()
