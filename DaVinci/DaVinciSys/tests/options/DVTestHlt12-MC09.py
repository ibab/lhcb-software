########################################################################
#
# $Id: DVTestHlt12-MC09.py,v 1.10 2010-03-05 10:58:05 pkoppenb Exp $
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
DaVinci().EvtMax = 1000                      # Number of events 
# DaVinci().SkipEvents = 650                       # Events to skip
DaVinci().PrintFreq  = 1                       # Print frequency
DaVinci().DataType = "MC09"                    # Pretend it's real data
DaVinci().Simulation   = True
from Configurables import HltCorrelations
DaVinci().MoniSequence = [ HltCorrelations() ]
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().Hlt = True             
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Feb10'   # Hlt2 for MC09
DaVinci().ReplaceL0BanksWithEmulated = True 
#
# a D* stream file from the first MC09 MB stripping 
#
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DSTAR.DST/00005436/0000/00005436_00000003_1.dstar.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
#
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

