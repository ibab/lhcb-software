from Gaudi.Configuration import *

from Configurables import Moore
Moore().ThresholdSettings = "Physics_pp_Draft2016"
Moore().UseTCK = False
Moore().ForceSingleL0Configuration = False
Moore().Split = ''
Moore().EnableTimer = True
# Moore().OutputLevel = INFO

from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool
LHCbTimingAuditor("TIMING").addTool(LHCbSequencerTimerTool, "TIMING")
#normalize the timing table in tests
LHCbTimingAuditor("TIMING").TIMING.Normalised = True

from PRConfig import TestFileDB
TestFileDB.test_file_db["2015NB_25ns_L0Filt0x0050"].run(configurable=Moore())
Moore().RemoveInputHltRawBanks = True

Moore().EvtMax = 1200

from Configurables import EventSelector
EventSelector().PrintFreq = 100

# Add your new HLT2 lines here to run them
# from Configurables import HltConf
# HltConf().AdditionalHlt2Lines = ['Hlt2MyNewLineA', 'Hlt2MyNewLineB']
