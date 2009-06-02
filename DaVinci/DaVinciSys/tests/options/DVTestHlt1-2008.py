########################################################################
#
# $Id: DVTestHlt1-2008.py,v 1.2 2009-06-02 15:13:57 jpalac Exp $
#
# Options for Hlt1 tests. Identical to Hlt1&2, except for type.
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
DaVinci().EvtMax = 100                          # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().HltType = 'Hlt1'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
EventSelector().Input   = [
    "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'",
    "   DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003401/0000/00003401_00000002_5.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
