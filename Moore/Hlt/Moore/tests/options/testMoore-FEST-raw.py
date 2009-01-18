#!/usr/bin/env gaudirun.py

Moore().hltType = 'Hlt1' # +Hlt2'
Moore().EvtMax = 2000
Moore().Simulation = True
Moore().DataType = '2008'
Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"

EventSelector().PrintFreq = 100
