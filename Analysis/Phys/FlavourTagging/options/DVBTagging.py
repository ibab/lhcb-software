########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2009-11-02
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

########################################################################
#
# If you want to import .opts options, do this first
#

########################################################################
#
# Example selection. 
#
from Configurables import DaVinci
DaVinci().MainOptions= "$DAVINCISYSROOT/tests/options/Do09selBu2LLK.py"

# /afs/cern.ch/lhcb/software/releases/DAVINCI/DAVINCI_v24r4/DaVinciSys

########################################################################
#
# Flavour tagging. 
#
from Configurables import BTagging, BTaggingTool, PhysDesktop, BTaggingChecker


tag = BTagging("BTagging")
tag.InputLocations = [ "Phys/Sel09Bu2LLK" ]
tag.OutputLevel = 1
BTaggingTool().OutputLevel = 1


####### Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "Sel09Bu2LLK" ]
tagcheck.TagsLocation = "Phys/Sel09Bu2LLK/FlavourTags"

######
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType   = "DC06" 
DaVinci().Simulation    = True
#DaVinci().UserAlgorithms = [ tag, tagcheck ]  # The algorithms
DaVinci().MoniSequence = [  tag  ]  # The algorithms
########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True  ## true to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True     
DaVinci().Hlt = False


########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bu2eeK-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

