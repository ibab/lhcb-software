########################################################################
#
# $Id: DVTestHlt12-MC09.py,v 1.13 2010-05-10 09:33:31 jpalac Exp $
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
#DaVinci().CondDBtag = 'MC09-20100430-vc-md100'
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
"   DATAFILE='/castor/cern.ch/grid/lhcb/MC/MC09/L0HLT1.DST/00005781/0000/00005781_00000757_1.l0hlt1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
#
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

