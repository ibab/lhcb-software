#!/usr/bin/env gaudirun.py
#
# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore.py
# or just
#   ../options/Moore.py
#
import Gaudi.Configuration
from Moore.Configuration import Moore

# if you want to generate a configuration, uncomment the following lines:
#Moore().generateConfig = True
#Moore().configLabel = 'Default'
#Moore().ThresholdSettings = 'Commissioning_PassThrough'
#Moore().configLabel = 'ODINRandom acc=0, TELL1Error acc=1'

Moore().ThresholdSettings = 'Physics_draft2012'

Moore().Verbose = True
Moore().EvtMax = 10000

Moore().UseDBSnapshot = False
Moore().DDDBtag    = 'hlt-20100906'
Moore().CondDBtag  = 'hlt-20100906'
Moore().Simulation = False
Moore().DataType   = '2010'
Moore().inputFiles  = [ 'castor:/castor/cern.ch/user/g/graven/raw/74733_0x0025_MB1.raw' ]
#Moore().inputFiles  = [ '/data/bfys/graven/74733_0x001a002b_MB1.raw' ]
from Configurables import HltRoutingBitsWriter
Moore().ForceSingleL0Configuration = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

#from Configurables import Hlt__Service
#Hlt__Service().Pedantic = False
