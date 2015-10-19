import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

Moore().ThresholdSettings = 'Physics_25ns_September2015'
Moore().ForceSingleL0Configuration = False
Moore().OutputLevel = 3
Moore().RemoveInputHltRawBanks = True
Moore().EvtMax = -1
Moore().EnableTimer = True

from Configurables import EventSelector
EventSelector().PrintFreq = 100

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input = test_file_db['MC2015_MinBias_SPD_lt_420_md_4xKee_L0Filtered']
# input.run(configurable=Moore())

Moore().CondDBtag = 'cond-20150828'
Moore().DDDBtag = 'dddb-20150724'
Moore().DataType = '2015'

files = ['/scratch/hlt/make_tcks/turcal_164699_0000000087.raw',
         '/scratch/hlt/make_tcks/turbo_164699_0000000088.raw',
         '/scratch/hlt/make_tcks/full_164699_0000000090.raw']
# base_dir = '/localdisk/hlt1/data2015_pNe_test'
# Moore().inputFiles = sorted([os.path.join(base_dir, f) for f in os.listdir(base_dir)])
Moore().inputFiles = files
# Moore().inputFiles = ['RedoL0_0x024E_01.mdf', 'RedoL0_0x024E_02.mdf']

from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().Online = True
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'

import sys
try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

Moore().outputFile = 'MakeTCK_physics.mdf'

from Configurables import HltConf
HltConf().Verbose = True
HltConf().RequireRoutingBits = [0x0, 0x4, 0x0]

import pprint
from GaudiPython.Bindings import AppMgr
from GaudiPython.Bindings import gbl

gaudi = AppMgr()
gaudi.initialize()
TES = gaudi.evtSvc()
writer = gaudi.algorithm("Writer")
writer.Enable = False

decisions = {}
while True:
    r = gaudi.run(1)
    if not TES['/Event']:
        break

    if not decisions:
        done = True
        for loc in ('Hlt2/DecReports', 'Hlt1/DecReports'):
            reports = TES[loc]
            if not reports:
                done = False
                break
            for name in reports.decisionNames():
                decisions[name] = False

        if not done:
            decisions = {}

    for loc in ('Hlt2/DecReports', 'Hlt1/DecReports'):
        reports = TES[loc]
        if not reports:
            continue
        for name in reports.decisionNames():
            report = reports.decReport(name)
            if report.decision() and not decisions[name]:
                print "Writing event for %s (%d/%d)" % (name, sum(decisions.itervalues()), len(decisions))
                decisions[name] = True
                writer.Enable = True
                writer._ialg.execute()
                writer.Enable = False

    if decisions and all(decisions.itervalues()):
        break

# from GaudiPython import InterfaceCast
# cast = InterfaceCast(gbl.IToolSvc)
# toolSvc = cast(gaudi.toolSvc().getInterface())
# print toolSvc.getInstances("DetailedMaterialLocator")

# hlt = gaudi.hltInspector()
# data = hlt.hltData()
# richProd = hlt.producer( richKey.str() )

# richCandidates = data.retrieve( richProd._ialg, richKey )

# from GaudiKernel.Configurable import applyConfigurableUsers
# applyConfigurableUsers()

# from Gaudi.Configuration import allConfigurables
# from GaudiPython.Bindings import AppMgr
# gaudi = AppMgr()
# gaudi.run(5000)
