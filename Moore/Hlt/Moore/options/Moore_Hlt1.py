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
Moore().EvtMax = 1000

Moore().UseDBSnapshot = False
Moore().ForceSingleL0Configuration = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

#from Configurables import Hlt__Service
#Hlt__Service().Pedantic = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.run(configurable=Moore()) 

# /data/bfys/graven/0x46
rel = lambda f : '/data/bfys/graven/0x46/'+f.split('/')[-1]
Moore().inputFiles = [ rel(fn) for fn in input.filenames ]
Moore().WriterRequires = [ 'Hlt1' ]
Moore().outputFile = '/data/bfys/graven/0x46/hlt1_reqhlt1.raw'

__replace = lambda orig, repl, members : [ m if m != orig else repl for m in members ]
def SplitHlt1() :
    from Configurables import GaudiSequencer as gs
    seq = gs('Hlt')
    seq.Members = __replace( gs('HltDecisionSequence'), gs('Hlt1'), seq.Members )

from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( SplitHlt1 )
