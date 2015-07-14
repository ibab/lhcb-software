import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

Moore().ThresholdSettings = 'Physics_draftEM2015'
Moore().ForceSingleL0Configuration = False
Moore().OutputLevel = 3
Moore().RemoveInputHltRawBanks = False
Moore().EvtMax = 5000
Moore().EnableTimer = True

from Configurables import EventSelector
EventSelector().PrintFreq = 100

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2012_raw_default']
# input = test_file_db['MC2015_MinBias_SPD_lt_420_md_4xKee_L0Filtered']
# input.run(configurable=Moore())

Moore().CondDBtag = 'cond-20150617'
Moore().DDDBtag = 'dddb-20150526'
Moore().DataType = '2015'

Moore().inputFiles = ['StripHLT_153759.mdf']

from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = True
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

import sys
try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

# Moore().CondDBtag = 'cond-20150409-1'
# Moore().DDDBtag = 'dddb-20150119-3'
# Moore().DataType = '2015'
# Moore().Split = 'Hlt2'

# Moore().inputFiles = ['/localdisk/hlt1/Run_0151833_20150504-100358.hlta0101.mdf']

# from Configurables import CondDB
# CondDB().IgnoreHeartBeat = True

# Moore().inputFiles = ['root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/MC2015_L0Processed/filtered/Charm/TCK_0xFF66/MagDown_27265101_Dst2D0Pi_D02KsKK_file0.dst',
#                       'root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/MC2015_L0Processed/filtered/Charm/TCK_0xFF66/MagDown_27265101_Dst2D0Pi_D02KsKK_file1.dst',
#                       'root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/MC2015_L0Processed/filtered/Charm/TCK_0xFF66/MagDown_27265101_Dst2D0Pi_D02KsKK_file2.dst',
#                       'root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/MC2015_L0Processed/filtered/Charm/TCK_0xFF66/MagDown_27265101_Dst2D0Pi_D02KsKK_file3.dst',
#                       'root://eoslhcb.cern.ch//eos/lhcb/wg/HLT/MC2015_L0Processed/filtered/Charm/TCK_0xFF66/MagDown_27265101_Dst2D0Pi_D02KsKK_file4.dst']

# from Configurables import HltConf
# HltConf().AdditionalHlt2Lines = (['Hlt2Run1TopoMu3BodyBBDT'])

from Configurables import HltConf
HltConf().Verbose = True

# from GaudiPython.Bindings import AppMgr
# from GaudiPython.Bindings import gbl
# import LoKiTrigger.Service

# StringKey = gbl.Gaudi.StringKey
# richKey   = StringKey( 'Hlt1CalibRICHMirrorDecision' )

# gaudi = AppMgr()
# gaudi.initialize()
# gaudi.run(1)

# from GaudiPython import InterfaceCast
# cast = InterfaceCast(gbl.IToolSvc)
# toolSvc = cast(gaudi.toolSvc().getInterface())
# print toolSvc.getInstances("DetailedMaterialLocator")

# hlt = gaudi.hltInspector()
# data = hlt.hltData()
# richProd = hlt.producer( richKey.str() )

# richCandidates = data.retrieve( richProd._ialg, richKey )

from GaudiKernel.Configurable import applyConfigurableUsers
applyConfigurableUsers()

from Gaudi.Configuration import allConfigurables
# from GaudiPython.Bindings import AppMgr
# gaudi = AppMgr()
# gaudi.run(5000)
