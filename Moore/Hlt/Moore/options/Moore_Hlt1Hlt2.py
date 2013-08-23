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
#Moore().EvtMax = 10000
from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().UseDBSnapshot = False
Moore().ForceSingleL0Configuration = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.filenames = [ '/data/bfys/graven/0x46/'+f.split('/')[-1] for f in input.filenames ]
input.run(configurable=Moore()) 
Moore().inputFiles = input.filenames

def hlt2DiMuonOnly() :
    __remove_re  = lambda re, members : [ m for m in members if not re.match(m.name()) ] 
    from Configurables import GaudiSequencer as gs
    import re
    seq = gs('Hlt2')
    seq.Members = __remove_re(re.compile('Hlt2(?!DiMuon).*') , seq.Members )
    #for c in walkAlgorithms( seq ) : c.OutputLevel = 2
    
from Gaudi.Configuration import appendPostConfigAction
appendPostConfigAction( hlt2DiMuonOnly )
