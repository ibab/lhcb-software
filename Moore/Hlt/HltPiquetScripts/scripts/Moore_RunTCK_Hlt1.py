import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

## Adapt these to your local environment
# Moore().generateConfig = True
# Moore().configLabel = 'van der Meer Scan May 2015'

## Moore().ThresholdSettings = 'VanDerMeerScan_2015'
Moore().UseTCK = True
Moore().InitialTCK = '0x00f7014D'
Moore().ForceSingleL0Configuration = True
Moore().OutputLevel = 3
Moore().EvtMax = -1
Moore().EnableTimer = True
Moore().CondDBtag = 'cond-20150617'
Moore().DDDBtag = 'dddb-20150526'
Moore().DataType = '2015'
Moore().CheckOdin = False
Moore().Simulation = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().Split = 'Hlt1'

Moore().inputFiles = ['StripHLT_153759.mdf']
Moore().outputFile = 'Moore_Hlt1.mdf'

from Configurables import CondDB
CondDB().Simulation = False
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

import AllHlt1
CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseSetCurrent = True
