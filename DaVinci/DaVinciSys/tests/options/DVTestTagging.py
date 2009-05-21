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
# Some selections. 
#

########################################################################
#
# Flavour tagging. 
#
importOptions( "$FLAVOURTAGGINGOPTS/BTaggingTool.py" )

from Configurables import BTagging, BTaggingChecker

tag = BTagging("BTagging")
tag.InputLocations = [ "Sel09Bu2LLK" ]
tag.OutputLevel = 3

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "BTagging" ]

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
#DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation    = True
DaVinci().MainOptions = "$DAVINCISYSROOT/tests/options/Do09selBu2LLK.py"
DaVinci().MoniSequence = [  tag, tagcheck ]  # The algorithms


########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bu2eeK-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
