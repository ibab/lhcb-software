#!/usr/bin/env gaudirun.py
#
# Place holder for testing the FSR info from an MDF
# At the moment I don't know quite what to write :S
# Just tests the Configurable, that's all
#
import os
from Gaudi.Configuration import *
import Configurables as Configs
from Configurables import LHCbApp, GaudiSequencer

#--- determine application to run
from LumiAlgs.Configuration import *

lSeq=GaudiSequencer("LumiSequence")

LumiAlgsConf().LumiSequencer=lSeq
LumiAlgsConf().OutputLevel=1

LHCbApp().EvtMax = 100

ApplicationMgr().TopAlg+=[lSeq]
