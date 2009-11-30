########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2009-02-19
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci

########################################################################
#
# Flavour tagging. 
#
from Configurables import BTagging, BTaggingTool, PhysDesktop, BTaggingChecker

tag = BTagging("BTagging")
tag.InputLocations = [ "/Event/Sel/Phys/Bu2LLK" ]
tag.OutputLevel = 2

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ "Sel09Bu2LLK" ]
tagcheck.TagsLocation = "/Event/Sel/Phys/Bu2LLK/FlavourTags"
tagcheck.OutputLevel = 3

MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
DaVinci().EvtMax     = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType   = "MC09" 
DaVinci().Simulation = True

########################################################################
from Configurables import PrintDecayTree

PrintDecayTree().InputLocations = [ "/Event/Sel/Phys/Bu2LLK"  ] 

DaVinci().MoniSequence = [ #PrintDecayTree(),
                            tag,
                            tagcheck
                         ]  # The algorithms

########################################################################
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/MC09-Bu2eeK/Bu2LLK-1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

