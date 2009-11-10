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
tag.InputLocations = [ "/Event/Sel/Phys/Bu2LLK" ]
# tag.OutputLevel = 1

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "Sel09Bu2LLK" ]
tagcheck.TagsLocation = "/Event/Sel/Phys/Bu2LLK/FlavourTags"

MessageSvc().Format = "% F%40W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType = "MC09"
DaVinci().Simulation    = True
# DaVinci().MainOptions = "$DAVINCISYSROOT/tests/options/Do09selBu2LLK.py"
from Configurables import StoreExplorerAlg, PrintDecayTree
PrintDecayTree().InputLocations = [ "/Event/Sel/Phys/Bu2LLK"  ] 
DaVinci().MoniSequence = [   PrintDecayTree()
                            #, StoreExplorerAlg() ] #
                            ,tag
                            , tagcheck
                            ]  # The algorithms


########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/MC09-Bu2eeK/Bu2LLK-1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

