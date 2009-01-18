#!/usr/bin/env gaudirun.py

Moore().hltType = 'Hlt1' # +Hlt2'
Moore().EvtMax = 2000
Moore().Simulation = False
Moore().DataType = '2008'
Moore().inputFiles= [ '/data/bfys/lhcb/data/2008/RAW/LHCb/PHYSICS_COSMICS/27804/027804_0000063303.raw' ]

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

EventSelector().PrintFreq = 100
