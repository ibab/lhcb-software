#!/usr/bin/env python
'''
Check some TES locations in a DST and print a summary of the number of events selected by each line.
Useage:
./check_dst_contents.py --input <some file name> --location <location of trunk - defailt "/Event">
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
    locationRoot = '/Event'
    opts, args = getopt.getopt(sys.argv[1:], "l:i", ["input=", "location="])

    for o, a in opts:
        if o in ("-i", "--input"):
            filename = a
        if o in ("-l", "--location") :
            locationRoot = a
            
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

    print '----------------------------------------------------------------------------------'
    print "Analysed ", nEvents, " events"

    for loc in nObjects.keys() :
        if nObjects[loc] > 0 :
            message = loc.ljust(length) + str('Events: '+ str(nSelEvents[loc])+'. Objects: '+ str(nObjects[loc])).rjust(10)
            print message
    print '----------------------------------------------------------------------------------'

