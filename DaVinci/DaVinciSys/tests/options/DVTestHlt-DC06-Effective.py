########################################################################
#
# $Id: DVTestHlt-DC06-Effective.py,v 1.2 2009-09-03 10:38:04 pkoppenb Exp $
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
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 200                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation   = True
DaVinci().MoniSequence = [ moni ] 
########################################################################
#
# HLT
#
DaVinci().ReplaceL0BanksWithEmulated = True    ## enable if you want to rerun L0
DaVinci().Hlt2Requires = 'L0'                  ## Run Hlt1 and 2
DaVinci().HltType = 'Hlt1+Hlt2'                ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().HltThresholdSettings = 'Effective_Nominal'
#
# DC06 stripped BB in order to get some statistics
#
EventSelector().Input   = [
    "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/phys-v3-lumi2/00001883/DST/0000/00001883_00002264_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
