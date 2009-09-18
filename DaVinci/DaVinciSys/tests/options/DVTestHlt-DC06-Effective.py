########################################################################
#
# $Id: DVTestHlt-DC06-Effective.py,v 1.3 2009-09-18 17:41:02 jpalac Exp $
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

#-- GAUDI jobOptions generated on Fri Sep 18 18:52:16 2009
#-- Contains event types : 
#--   10000000 - 10 files - 17852 events - 11.10 GBytes
#-- DC06-Sim + DC06-Reco_v30 + DC06-Stripping_v31
EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000857_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000858_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000859_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000860_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000861_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000862_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000867_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000874_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000924_3.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbdata&castorVersion=2&path=/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000926_3.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs = [ 'xmlcatalog_file:DC06-Stripped-inclb-unbiased.xml' ]

########################################################################
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
