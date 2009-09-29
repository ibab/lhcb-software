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
from Configurables       import HltSelReportsMaker


# if you want to generate a configuration, uncomment the following lines:
Moore().generateConfig = True
Moore().configLabel = 'ODINRandom accept=1, TELL1 Error accept=0'

Moore().HltType = 'Hlt1+Hlt2'
Moore().ThresholdSettings = 'Effective_FEST'

Moore().Verbose = True


Moore().DDDBtag   = 'MC09-20090602'
Moore().CondDBtag = 'sim-20090402-vc-md100'
Moore().Simulation = True
Moore().DataType   = '2009'
Moore().inputFiles = [ '/data/bfys/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
#Moore().inputFiles = [ 'castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_%08d_1.mdf'%f  for f in range(1,4) ]
#Moore().outputFile = '/tmp/foo.mdf'

EventSelector().PrintFreq = 100
Moore().EvtMax = 10000

# Moore().RunOnline = True
