########################################################################
#
# $Id: DVTestHlt-2008.py,v 1.3 2009-02-23 20:10:31 pkoppenb Exp $
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
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True
########################################################################
#
# HLT
#
DaVinci().Hlt2Requires = 'L0'
DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().Input   = [
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000003_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000004_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000005_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003400/0000/00003400_00000006_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

