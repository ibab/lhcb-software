#!/usr/bin/env gaudirun.py

from Moore.Configuration import Moore

Moore().UseDBSnapshot = False
Moore().Simulation = False
Moore().DataType = '2009'
Moore().ThresholdSettings = 'Physics_MinBiasL0_PassThroughHlt_Jan10'

# correct data file
Moore().inputFiles= [ "castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/COLLISION09/63949/063949_0000000001.raw" ]
#Moore().inputFiles= [                 '/data/bfys/lhcb/data/2009/RAW/FULL/LHCb/COLLISION09/63949/063949_0000000001.raw' ]


from Configurables import EventSelector
EventSelector().PrintFreq = 1000


