########################################################################
#
# $Id: DVTestHlt1-MC09.py,v 1.1 2009-06-22 15:24:11 pkoppenb Exp $
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
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "MC09"                    # Default is "DC06"
DaVinci().Simulation   = True
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().HltType = 'Hlt1'             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
#-- GAUDI jobOptions generated on Mon Jun 22 16:57:21 2009
#-- Contains event types : 
#--   11144001 - 15 files - 215879 events - 62.23 GBytes
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004875/0000/00004875_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
