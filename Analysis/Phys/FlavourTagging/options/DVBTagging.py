########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2009-02-19
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

########################################################################
#
# If you want to import .opts options, do this first
#
importOptions("$STDOPTS/PreloadUnits.opts")

########################################################################
#
# Example selection. 
#
#importOptions("$B2DILEPTONROOT/options/DoPreselBu2LLK.opts")
#preselSeq = GaudiSequencer("SeqPreselBu2LLK")
importOptions( "$FLAVOURTAGGINGOPTS/DC06SelBs2DsPi.opts" )
preselSeq = GaudiSequencer("SeqDC06SelBs2DsPi")

########################################################################
#
# Flavour tagging. 
#
from Configurables import BTagging, BTaggingTool, PhysDesktop, BTaggingChecker

#btool = BTaggingTool()
#btool.OutputLevel = 2

tag = BTagging("BTagging")
tag.InputLocations = [ "Phys/DC06SelBs2DsPi" ]
tag.OutputLevel = 3

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "Phys/BTagging" ]

MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 500                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType = "DC06"                    # Default is "DC06"
DaVinci().Simulation    = True
DaVinci().UserAlgorithms = [ preselSeq, tag, tagcheck ]  # The algorithms
########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True  ## true to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True     
DaVinci().HltType = ''  ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'


########################################################################
#
# example data file
#
EventSelector().Input   = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002034/DST/0000/00002034_00000001_2.dst' TYP='POOL_ROOTTREE' OPT='READ'"    ]

