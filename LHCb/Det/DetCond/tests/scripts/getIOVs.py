#!/usr/bin/env python
"""
Script for the test of IOV retrieval
"""
__author__ = "Marco Clemencic"

import sys
from datetime import datetime, timedelta

def toTimeStamp(dt):
    if isinstance(dt, timedelta):
        t = dt
    else:
        t = dt - datetime(1970, 1, 1, 0)
    return (t.days * 60 * 60 * 24 + t.seconds) * 1000000000

def toDateTime(ts):
    return datetime(1970, 1, 1, 0) + timedelta(seconds=ts/1000000000)

def configure():
    from Gaudi.Configuration import (ApplicationMgr,
                                     MessageSvc, ERROR)
    from Configurables import DDDBConf, CondDB, CondDBAccessSvc, EventClockSvc, FakeEventTime
    dddbConf = DDDBConf()
    cdb = CondDB()

    cdb.PartitionConnectionString["DQFLAGS"] = "sqlite_file:../data/DQFLAGS.db/DQFLAGS"
    cdb.Tags["DQFLAGS"] = ""

    ecs = EventClockSvc(InitialTime=toTimeStamp(datetime(2012,1,1,12)))
    ecs.addTool(FakeEventTime, "EventTimeDecoder")
    ecs.EventTimeDecoder.StartTime = ecs.InitialTime
    ecs.EventTimeDecoder.TimeStep = toTimeStamp(timedelta(days=1))

    ApplicationMgr(TopAlg = ["LoadDDDB"], EvtSel = "NONE")
    MessageSvc(OutputLevel = ERROR)

def checkIOVs(dbReader, since, until, expected):
    import GaudiPython
    IOV = GaudiPython.gbl.ICondDBReader.IOV
    Time = GaudiPython.gbl.Gaudi.Time

    print "Checking %s -> %s ..." % (since, until),
    t1 = Time(toTimeStamp(since))
    t2 = Time(toTimeStamp(until))
    iov = IOV(t1, t2)
    result = dbReader.getIOVs("/Conditions/DQ/Flags", iov, 0)

    found = [(toDateTime(iov.since.ns()), toDateTime(iov.until.ns())) for iov in result]

    good = found == expected
    if not good:
        print "ERROR"
        print "  expected:", [tuple(map(str, iov)) for iov in expected]
        print "  found:   ", [tuple(map(str, iov)) for iov in found]
    else:
        print "OK"
    return good

def main():
    configure()

    from Gaudi.Configuration import configurationDict
    from pprint import pprint
    import GaudiPython
    app = GaudiPython.AppMgr()
    pprint(configurationDict())
    app.initialize()

    dq = app.service("DQFLAGS", GaudiPython.gbl.ICondDBReader)

    dbData = [(datetime(2012,1,1,0), datetime(2012,1,2,0)),
              (datetime(2012,1,2,0), datetime(2012,1,3,0)),
              (datetime(2012,1,3,0), datetime(2012,1,4,0)),
              (datetime(2012,1,5,0), datetime(2012,1,6,0))]

    tests = [(datetime(2012,1,1,0),  datetime(2012,1,4,0),  dbData[0:3]),
             (datetime(2012,1,1,12), datetime(2012,1,3,12), dbData[0:3]),
             (datetime(2012,1,2,12), datetime(2012,1,5,12), dbData[1:4]),
             (datetime(2012,1,4,12), datetime(2012,1,6,12), dbData[3:4]),
             ]
    print "\n=== Begin Tests ==="
    bad = 0
    for since, until, expected in tests:
        if not checkIOVs(dq, since, until, expected):
            bad += 1
    print "=== End Tests ==="

    if bad:
        print "\nFailed %d tests out of %d\n" % (bad, len(tests))
        sys.exit(1)

    print ""

if __name__ == '__main__':
    main()
