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

Moore().ThresholdSettings = 'Physics_September2012'

Moore().EvtMax = 1200

Moore().UseDBSnapshot = False
Moore().ForceSingleL0Configuration = False

from PRConfig.TestFileDB import test_file_db
input = test_file_db['2012_raw_default']
input.run(configurable=Moore()) 

from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor('TIMER').addTool(SequencerTimerTool, name="TIMER")
TimingAuditor('TIMER').TIMER.NameSize=150
