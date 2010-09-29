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
location = "/Event/Strip/Phys/StripBu2eeK"

from Configurables import BTagging, BTaggingChecker

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
# tag.OutputLevel = 1

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2010"
DaVinci().Simulation    = True
from Configurables import StoreExplorerAlg, PrintDecayTree
PrintDecayTree().InputLocations = [ location ] 
DaVinci().MoniSequence = [   PrintDecayTree()
                            ,tag
                            , tagcheck
                            ]  # The algorithms


########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00006522/0000/00006522_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"

