import Gaudi.Configuration
from Moore.Configuration import Moore

Moore().ThresholdSettings = "Physics_pp_Draft2016"
Moore().UseTCK = False
Moore().ForceSingleL0Configuration = False
Moore().EvtMax = 1200

from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool
LHCbTimingAuditor("TIMING").addTool(LHCbSequencerTimerTool,"TIMING")
#normalize the timing table in tests
LHCbTimingAuditor("TIMING").TIMING.Normalised = True

from PRConfig import TestFileDB
TestFileDB.test_file_db["2015NB_25ns_L0Filt0x0050"].run(configurable=Moore())
Moore().RemoveInputHltRawBanks = True
