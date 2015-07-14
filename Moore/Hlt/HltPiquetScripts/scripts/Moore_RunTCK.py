import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

## Adapt these to your local environment
# Moore().generateConfig = True
# Moore().configLabel = 'van der Meer Scan May 2015'

## Moore().ThresholdSettings = 'VanDerMeerScan_2015'
Moore().UseTCK = True
Moore().InitialTCK = '0x00f5014d'
Moore().OutputLevel = 3
Moore().EvtMax = -1
Moore().EnableTimer = True
# Moore().RemoveInputHltRawBanks = True

# Debugging some events
# Moore().SkipEvents = 11320


from Configurables import EventSelector
EventSelector().PrintFreq = 100

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input.run(configurable=Moore())
# Moore().Split = 'Hlt1'

# from Configurables import HltConfigSvc
# HltConfigSvc().Hlt2Mode = False
# HltConfigSvc().OutputLevel = 3

Moore().CheckOdin = False

Moore().inputFiles = ['StripHLT_153759.mdf']

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True
CondDB().UseDBSnapshot = True
# CondDB().DBSnapshotDirectory = "/home/raaij/db_snapshot"
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

import sys
try:
    import All
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import All

CondDB().RunChangeHandlerConditions = All.ConditionMap

Moore().CondDBtag = 'cond-20150601'
Moore().DDDBtag = 'dddb-20150526'
Moore().DataType = '2015'

# GaudiPython
# from GaudiPython import AppMgr
# gaudi = AppMgr()
# gaudi.initialize()
# TES = gaudi.evtSvc()

# n = 0
# while n < 20:
#     r = gaudi.run(1)
#     if not TES['/Event']:
#         break

#     selReports = TES['Hlt2/SelReports']
#     odin = TES['DAQ/ODIN']
#     evt = (odin.runNumber(), odin.eventNumber())
#     if evt == (154478, 79698043):
#         print 'Found the event!'
#         break

#     # print odin.runNumber(), odin.eventNumber()
#     # print selReports
#     # print '\n\n\n'
#     n += 1
