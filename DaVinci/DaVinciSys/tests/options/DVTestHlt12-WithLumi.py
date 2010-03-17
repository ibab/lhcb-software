########################################################################
#
# $Id: DVTestHlt12-WithLumi.py,v 1.3 2010-03-17 18:44:33 pkoppenb Exp $
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
DaVinci().EvtMax = 10000                       # Number of events 
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "2009"                    # Pretend it's real data
########################################################################
#
# HLT
#
DaVinci().Hlt = True             
DaVinci().HltThresholdSettings = 'Physics_MinBiasL0_PassThroughHlt_Feb10'   # Do almost nothing
DaVinci().ReplaceL0BanksWithEmulated = True 
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#
# first contains lumi events, second doesn't
#
DaVinci().Input   = [
"   DATAFILE='PFN:/castor/cern.ch/user/s/smenzeme/DATA2009/run63683-63690_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/user/p/pkoppenb/DATA2009/run63445-63497_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'"
] 
