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
from Configurables import DaVinci
DaVinci().MainOptions= "$DAVINCISYSROOT/tests/options/Do09selBu2LLK.py"


########################################################################
#
# Flavour tagging. 
#
from Configurables import BTagging, BTaggingTool, PhysDesktop, BTaggingChecker

#btool = BTaggingTool()
#btool.OutputLevel = 2

tag = BTagging("BTagging")
tag.InputLocations = [ "Sel09Bu2LLK" ]
tag.OutputLevel = 3

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "BTagging" ]

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
DaVinci().MoniSequence = [  tag, tagcheck ]  # The algorithms
########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True  ## true to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True     
DaVinci().HltType = ''  ## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'


########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bu2eeK-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

