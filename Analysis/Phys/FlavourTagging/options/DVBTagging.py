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
from Configurables import BTagging, BTaggingChecker

location = "/Event/Strip/Phys/StripBu2eeK"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
tag.OutputLevel = 2

# Flavour tagging Checker:
tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"


MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
DaVinci().EvtMax     = 200                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType   = "MC09" 
DaVinci().Simulation = True

########################################################################

DaVinci().MoniSequence = [  tag,
                            tagcheck
                         ]  # The algorithms

########################################################################
#/releases/DAVINCI/DAVINCI_v24r7/DaVinciSys/tests/options/DVTestTagging.py
#
# example data file
#
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/p/pkoppenb/MC09-Bu2eeK/Sel.Belectron-440-0.dst' TYP='POOL_ROOTTREE' OPT='READ'"  ]

