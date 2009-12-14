########################################################################
#
# $Id: DVTestHlt-DC06-None-NoH.py,v 1.1 2009-12-14 16:52:28 pkoppenb Exp $
#
# Options for a typical DaVinci job
#
# @author Patrick Koppenburg
# @date 2008-08-06
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer
########################################################################
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELCHECKERROOT/options/Hlt2Correlations.py")
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 200                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 1                     # Print frequency
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().MoniSequence = [ moni ] 
# DaVinci().HistogramFile = "Test.root"
########################################################################
#
# HLT
#
DaVinci().ReplaceL0BanksWithEmulated = True    ## enable if you want to rerun L0
DaVinci().Hlt = True 
DaVinci().Hlt2Requires = 'L0'                  ## Run Hlt1 and 2
DaVinci().HltThresholdSettings = ''            ## None !

#-- GAUDI jobOptions generated on Fri Sep 18 18:52:16 2009
#-- Contains event types : 
#--   10000000 - 10 files - 17852 events - 11.10 GBytes
#-- DC06-Sim + DC06-Reco_v30 + DC06-Stripping_v31
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000857_3.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

