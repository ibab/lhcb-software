#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *
from HltConf.HltVeloLines     import HltVeloLinesConf


# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'NO prescale'

Moore().hltType = 'Hlt1' # +Hlt2'
Moore().Verbose = True

# FEST specific options...
HltConf().LumiBankKillerAcceptFraction = 0.9999
HltVeloLinesConf().Prescale = 1

Moore().Simulation = True
Moore().DataType   = '2008'
#Moore().inputFiles = [ '/data/bfys/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]

EventSelector().PrintFreq = 100
Moore().EvtMax = 1000

LHCbApp().DDDBtag   = "default"
LHCbApp().CondDBtag = "default"
