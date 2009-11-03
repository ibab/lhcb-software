########################################################################
#
# $Id: DVTestHlt12-MC09-StrippedWithLumi.py,v 1.1 2009-11-03 16:25:10 pkoppenb Exp $
#
# Test for Hlt1&2. This should be identical to Hlt1 except for the Hlt settings
#
# The Hlt1 efficiency should be identical to the one in DVTestHlt1-2008.py
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
DaVinci().EvtMax = -1                          # Number of events 
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "MC09"                    # Pretend it's real data
DaVinci().Simulation   = True
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().Hlt = True             
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09'   # Hlt2 for MC09
#
# first contains lumi events, second doesn't
#
EventSelector().Input   = [
 "   DATAFILE='castor:/castor/cern.ch/user/p/pkoppenb/HLT/lumi/L0xHlt1_2ff68.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
#  "   DATAFILE='castor:/castor/cern.ch/user/p/pkoppenb/HLT/lumi/L0xHlt1_3ff68.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
