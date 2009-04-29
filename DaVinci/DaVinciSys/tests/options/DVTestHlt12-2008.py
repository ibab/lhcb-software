########################################################################
#
# $Id: DVTestHlt12-2008.py,v 1.1 2009-04-29 09:25:03 pkoppenb Exp $
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
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True
########################################################################
#
# HLT
#
DaVinci().Hlt2Requires = 'L0'
DaVinci().HltType = 'Hlt1+Hlt2'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
EventSelector().Input   = [
    "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
