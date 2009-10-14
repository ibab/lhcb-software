########################################################################
#
# $Id: DVTestHlt12-MDF.py,v 1.7 2009-10-14 14:33:40 pkoppenb Exp $
#
# Test for Hlt1&2. Test for MDF files.
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().HltThresholdSettings = 'Miriam_20090430'
DaVinci().EvtMax = 10000                       # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "2009"                    # Default is "MC09", pretend it's real data
# DaVinci().Simulation   = False 
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().Hlt = True             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().InputType = "MDF"
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'
EventSelector().Input   = [
    "   DATA='PFN:castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/FEST/FEST/47910/047910_0000000001.raw' SVC='LHCb::MDFSelector'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []

appendPostConfigAction(noInit)
