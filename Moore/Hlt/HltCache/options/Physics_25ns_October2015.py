import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

## Adapt these to your local environment
# Moore().generateConfig = True
# Moore().configLabel = 'van der Meer Scan May 2015'

Moore().ThresholdSettings = 'Physics_25ns_October2015'
Moore().UseTCK = False
Moore().OutputLevel = 3
Moore().EvtMax = 5000
Moore().EnableTimer = False
Moore().CheckOdin = False
Moore().Simulation = False
Moore().ForceSingleL0Configuration = True
Moore().SkipEvents = 0
Moore().CondDBtag = 'cond-20150828'
Moore().DDDBtag = 'dddb-20150724'

from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().Split = ''

# from PRConfig.TestFileDB import test_file_db
# input = test_file_db['2015HLTValidationData_L0filtered_0x0050']
# input.run(configurable=Moore())
