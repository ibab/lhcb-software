########################################################################
#
# $Id: DVTestHlt12-MDF.py,v 1.2 2009-05-01 17:54:59 pkoppenb Exp $
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
DaVinci().DataType = "2008"                    # Default is "DC06"
# DaVinci().Simulation   = False 
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().Hlt2Requires = 'L0+Hlt1'
DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().InputType = "MDF"
EventSelector().Input   = [
    "   DATA='PFN:castor:/castor/cern.ch/grid/lhcb/test/MDF/00003083/0000/00003083_00000001_1.mdf' SVC='LHCb::MDFSelector'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []

appendPostConfigAction(noInit)
