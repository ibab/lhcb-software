import os
import Gaudi.Configuration
from Moore.Configuration import Moore
from GaudiKernel.SystemOfUnits import MeV, mm, m

Moore().ThresholdSettings = 'Physics_pp_Draft2016'
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
