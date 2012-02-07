#!/usr/bin/env python
'''
Check the DecReports in a DST and print a summary of the number of events selected by each line.
Useage:
./check_decreports.py --input <some file name> --location <TES location of DecReports - Default /Event/Strip/Phys/DecReports>
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

    parser.remove_option('--branch')
    parser.remove_option('--root')
    parser.add_option('-l', '--location',
                      type='string',
                      dest='location',
                      default= '/Event/Strip/Phys/DecReports',
                      help='TES Location of DecReports container [default: %default]')
    parser.set_defaults(output='decreports.txt')
    
    (options, args) = parser.parse_args()

    if len(args) != 1 :
        parser.error('expected one positional argument')

    filename = args[0]
    location = options.location
    output = options.output
    verbose = options.verbose
    nevents = options.nevents

    if verbose :
        print 'will look for DecReports in', location
        print 'nevents', nevents
        print 'output file', output
    
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
    nDecReports = 0
    decReportSummary = {}


    while ( nextEvent() ) :
        nEvents+=1
        decReports = evtSvc[location]
        if decReports :
            nDecReports += 1
            for decName in decReports.decisionNames() :
                decision = decReports.decReport(decName)
                if decision :
                    addEntry(decReportSummary, decName, decision.decision())

    _printMessage = ''
    for decs in decReportSummary.iteritems() :
        message = str(decs[0]) + " : " + str(decs[1]) + '\n'
        _printMessage += message
        outputFile.write(message)

    if verbose :
        print '======================================================================================'
        print 'Found', nDecReports, 'DecReports in', nEvents, 'events'    
        print '======================================================================================'
        print _printMessage
        print '======================================================================================'
        print 'Wrote summary to', output

    outputFile.close()
