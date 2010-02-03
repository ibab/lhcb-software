########################################################################
#
# $Id: DVTestHlt12-WithLumi.py,v 1.2 2010-02-03 10:00:23 pkoppenb Exp $
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
DaVinci().EvtMax = 100000                       # Number of events 
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "2009"                    # Pretend it's real data
########################################################################
#
# HLT
#
DaVinci().Hlt = True             
DaVinci().HltThresholdSettings = 'Debug_10000Vis_1000L0_Velo_Hlt1_Oct09'   # Do almost nothing
DaVinci().ReplaceL0BanksWithEmulated = True
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# first contains lumi events, second doesn't
#
DaVinci().Input   = [
"   DATAFILE='PFN:/castor/cern.ch/user/s/smenzeme/DATA2009/run63683-63690_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/user/p/pkoppenb/DATA2009/run63445-63497_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'"
] 
