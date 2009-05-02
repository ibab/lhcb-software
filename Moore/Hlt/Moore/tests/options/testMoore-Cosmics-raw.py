#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Moore.Configuration import *

Moore().HltType = 'Hlt1' # +Hlt2'
Moore().EvtMax = 2000
Moore().Simulation = False
Moore().DataType = '2008'
Moore().inputFiles= [ 'castor:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

EventSelector().PrintFreq = 100
