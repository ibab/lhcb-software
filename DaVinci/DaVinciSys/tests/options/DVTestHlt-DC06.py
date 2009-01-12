########################################################################
#
# $Id: DVTestHlt-DC06.py,v 1.1 2009-01-12 11:05:20 pkoppenb Exp $
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
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 10                      # Print frequency
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation   = True
########################################################################
#
# HLT
#
DaVinci().ReplaceL0BanksWithEmulated = True    ## enable if you want to rerun L0
DaVinci().Hlt2IgnoreHlt1Decision = False       ## enable if you want Hlt2 irrespective of Hlt1
DaVinci().HltType = 'Hlt1+Hlt2'                ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().Input   = [
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000002_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000003_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000004_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000005_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000006_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000007_1.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/L0-v1-lumi2/00001959/DST/0000/00001959_00000008_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]


