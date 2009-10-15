########################################################################
#
# $Id: DVTestHlt12-MC09.py,v 1.5 2009-10-15 09:25:06 pkoppenb Exp $
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
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq  = 100                     # Print frequency
DaVinci().DataType = "MC09"                    # Pretend it's real data
DaVinci().Simulation   = True
# DaVinci().MoniSequence += [ ReadHltSummary() ]
########################################################################
#
# HLT
#
DaVinci().Hlt2Requires = 'L0'
DaVinci().Hlt = True             ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_EffectiveHlt2_Jul09'  ## need to replace with MC09
#-- GAUDI jobOptions generated on Mon Jun 22 16:57:21 2009
#-- Contains event types : 
#--   11144001 - 15 files - 215879 events - 62.23 GBytes
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004875/0000/00004875_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
#
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
