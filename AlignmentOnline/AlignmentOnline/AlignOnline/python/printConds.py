#!/usr/bin/env python
import os
import argparse
parser = argparse.ArgumentParser(usage = 'usage: %prog <2011|2012>')

parser.add_argument("start", nargs = 1, metavar = 'start time',
                    type = int)
parser.add_argument("run", nargs = 1, metavar = 'run number',
                    type = int)
parser.add_argument("output", nargs = 1, type = str)
parser.add_argument("--online", action = "store_true",
                   dest = "online", default = False,
                   help = "Configure for online running" )

arguments = parser.parse_args()

from Gaudi.Configuration import *
import os, sys, fnmatch, re
from GaudiKernel.ProcessJobOptions import importOptions

# Get list conditions
try:
    from CondMap import ConditionMap
except ImportError:
    from All import ConditionMap
list_conditions = sum(ConditionMap.values(),[])

# Configure Brunel
from Configurables import LHCbApp
app = LHCbApp()
app.DataType = '2015'
app.Simulation = False
app.EvtMax = 1

import HLT2Params

app.DDDBtag = HLT2Params.DDDBTag
app.CondDBtag = HLT2Params.CondDBTag

from Configurables import EventClockSvc, FakeEventTime, EventDataSvc
ecs = EventClockSvc()
ecs.InitialTime = arguments.start[0]
ecs.addTool(FakeEventTime, "EventTimeDecoder")
ecs.EventTimeDecoder.TimeStep = 10
ecs.EventTimeDecoder.StartTime = arguments.start[0]

from Configurables import DumpConditions
DumpConditions().RunStartTime = arguments.start[0]
DumpConditions().RunNumber = arguments.run[0]
DumpConditions().OutFile= arguments.output[0]
DumpConditions().Conditions = list_conditions

from Configurables import CondDB
cdb = CondDB()
if arguments.online:
    import CondMap
    cdb.RunChangeHandlerConditions = CondMap.ConditionMap
    cdb.EnableRunChangeHandler = True
    cdb.UseDBSnapshot = True
    cdb.Tags = { "DDDB"     : HLT2Params.DDDBTag,
                 "LHCBCOND" : HLT2Params.CondDBTag,
                 "ONLINE"   : 'fake'}
    cdb.Online = True
else:
    cdb.EnableRunStampCheck = False 
    #cdb.IgnoreHeartBeat = True

    useNewFeature = False
    if useNewFeature:
        # new feature Liang
        cdb.UseOracle = True
        cdb.Online = True
    else:
        # Marco's workaround
        from Gaudi.Configuration import appendPostConfigAction
        from Configurables import CondDBDispatcherSvc, CondDBAccessSvc
        
        def oracle_online():
            oo = CondDBAccessSvc('ONLINE_ORACLE')
            oo.ConnectionString = 'CondDBOnline/ONLINE'
            CondDBDispatcherSvc('MainCondDBReader').Alternatives['/Conditions/Online'] = oo
            
        appendPostConfigAction(oracle_online)
        os.environ['CORAL_DBLOOKUP_PATH'] = os.environ['CORAL_AUTH_PATH'] = '/group/online/condb_viewer'


    from Configurables import DumpConditions, GaudiSequencer
dumpSeq = GaudiSequencer("DumpSequence", Members = [DumpConditions()])
ApplicationMgr().TopAlg = [dumpSeq]
ApplicationMgr().ExtSvc += ["IncidentSvc"]
ApplicationMgr().EvtSel = "NONE"

# Run
from GaudiPython.Bindings import AppMgr
gaudi = AppMgr()
for meth, args in (('initialize' , None), ('run' , [1]), ('stop' , None),
                   ('finalize' , None), ('exit', None)):
    m = getattr(gaudi, meth)
    if args:
        status = m(*args)
    else:
        status = m()
    if status.isFailure():
        print 'Failed in ' + meth
        sys.exit(1)

