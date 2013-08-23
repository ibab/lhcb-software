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

Moore().ThresholdSettings = 'Physics_September2012'

Moore().Verbose = True
Moore().EvtMax = 10000
from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().UseDBSnapshot = False
Moore().ForceSingleL0Configuration = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.run(configurable=Moore()) 
Moore().inputFiles = [ '/data/bfys/graven/0x46/hlt1.raw' ]

Moore().Split = 'Hlt2'
