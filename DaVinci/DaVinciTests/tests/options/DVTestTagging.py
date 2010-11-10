########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2010-09-29
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

########################################################################
#
# Some selections. 
#
from Configurables import CheatedSelection, PhysDesktop

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.InputLocations = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 4

########################################################################
#
# Flavour tagging. 
#
location = "Phys/CheatedSelection"

from Configurables import BTagging, BTaggingTool, BTaggingChecker

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
tag.OutputLevel = 3

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP" #needed by CheatedSel


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
DaVinci().EvtMax     = 500                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2010"
DaVinci().CondDBtag = 'sim-20101026-vc-md100'
DaVinci().Simulation    = True
from Configurables import StoreExplorerAlg, PrintDecayTree
PrintDecayTree().InputLocations = [ location ] 
DaVinci().MoniSequence = [   #PrintDecayTree()
                             cheatsel
                            ,tag
                            ,tagcheck
                            ]  # The algorithms


########################################################################
#
# example data file
# bsjpsiphi
DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00006522/0000/00006522_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
#DaVinci().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00007386/0000/00007386_00000244_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]



