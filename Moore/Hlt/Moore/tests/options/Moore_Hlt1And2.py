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

Moore().EvtMax = 1200

Moore().ForceSingleL0Configuration=False

from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool

LHCbTimingAuditor("TIMING").addTool(LHCbSequencerTimerTool,"TIMING")

#normalize the timing table in tests
LHCbTimingAuditor("TIMING").TIMING.Normalised=True
