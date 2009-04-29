########################################################################
#
# $Id: DVTestHlt-DC06.py,v 1.6 2009-04-29 09:25:03 pkoppenb Exp $
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
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1                          # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation   = True
########################################################################
#
# HLT
#
DaVinci().ReplaceL0BanksWithEmulated = True    ## enable if you want to rerun L0
DaVinci().Hlt2Requires = 'L0'                  ## Run Hlt1 and 2
DaVinci().HltType = 'Hlt1+Hlt2'                ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().Input   = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000002_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000003_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
