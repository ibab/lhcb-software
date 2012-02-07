#!/usr/bin/env python

from LHCbKernel import Counters

def dumpGEC(gec) :
    for k, v in gec.iteritems() :
        print k, ': ', v

if __name__ == '__main__' :

    # deal with command-line arguments
    
    from DSTWriterTests.default_args import parser
    parser.set_defaults(output='gec.txt')
    (options, args) = parser.parse_args()
        
    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    locationRoot = options.root
    output = options.output
    verbose = options.verbose
    nevents = options.nevents
    gecLoc = options.branch + '/GlobalEventCounters'
    gecLoc.replace('//', '/')
    # Configuration

    from Configurables import LHCbApp
    from Configurables import MessageSvc
 
    MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
    
    from Gaudi.Configuration import EventSelector
    EventSelector().PrintFreq=100

    lhcbApp = LHCbApp()
    lhcbApp.DDDBtag = 'default'
    lhcbApp.CondDBtag = 'default'

    # GaudiPython

    from GaudiPython.Bindings import AppMgr
    import GaudiPython
    appMgr = AppMgr(outputlevel=3)
    appMgr.config( files = ['$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts'])
    appMgr.ExtSvc += ['DataOnDemandSvc']
    appMgr.initialize()

    evtSvc = appMgr.evtSvc()
    evtSel = appMgr.evtSel()
    tsvc= appMgr.toolsvc()

    from AnalysisPython import Functors
    nextEvent = Functors.NextEvent(appMgr, EvtMax=nevents)

    # open a DST or MicroDST
    evtSel.open(filename)

    nEvents=0

    while ( nextEvent() ) :
        nEvents+=1
        gec = evtSvc[gecLoc]
        dumpGEC(gec)
