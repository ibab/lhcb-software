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

    import sys, getopt
    from GaudiConf.Configuration import *
    from AnalysisPython import Dir, Functors
    from GaudiPython.Bindings import gbl, AppMgr, Helper

    decReportsLocation = '/Event/Strip/Phys/DecReports'
    filename = ''
    output = 'decreports.txt'
    opts, args = getopt.getopt(sys.argv[1:], "l:i:o", ["input=", "location=", "output="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        elif o in ('-l', '--location') :
            decReportsLocation = a
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
    nDecReports = 0
    decReportSummary = {}


    while ( nextEvent() ) :
        nEvents+=1
        decReports = evtSvc[decReportsLocation]
        if decReports :
            nDecReports += 1
            for decName in decReports.decisionNames() :
                decision = decReports.decReport(decName)
                if decision :
                    addEntry(decReportSummary, decName, decision.decision())

    print '======================================================================================'
    print 'Found', nDecReports, 'DecReports in', nEvents, 'events'
    print '======================================================================================'
    for decs in decReportSummary.iteritems() :
        message = str(decs[0]) + " : " + str(decs[1])
        print message
        outputFile.write(message+'\n')
    print '======================================================================================'
    print 'Wrote summary to', output
    outputFile.close()
