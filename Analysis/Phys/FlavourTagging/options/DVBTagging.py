########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marc Grabalosa
# @date 2010-02-20
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci

######################################################################\
##
# CheatedSelection.

from Configurables import CheatedSelection

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.InputLocations = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 4


########################################################################
#
# Flavour tagging. 
#
from Configurables import BTagging, BTaggingTool, BTaggingChecker, TriggerTisTos, TaggingUtils, MCMatchObjP2MCRelator, BackgroundCategory

location = "Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]

tag.OutputLevel    = 3
tag.addTool( TriggerTisTos )
tag.TriggerTisTos.OutputLevel = 4
tag.addTool( TaggingUtils )
tag.TaggingUtils.OutputLevel = 4
tag.addTool( MCMatchObjP2MCRelator )
tag.MCMatchObjP2MCRelator.OutputLevel = 4
tag.addTool( BackgroundCategory )
tag.BackgroundCategory.OutputLevel = 4

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP" #needed by CheatedSel
tag.BTaggingTool.UseReFitPV = True
#tag.BTaggingTool.ForceSignalID = "Bs" #"Bs", "Bd", "Bu", will Force Signal ID
#tag.BTaggingTool.CombineTaggersName = "CombineTaggersNN" #combine taggers with NN
tag.BTaggingTool.OutputLevel  = 4

#Import cuts
from FlavourTagging.Tunings import TuneTool
TuneTool(tag,"MC10") #for example

######################################################################\
##
# Flavour tagging Checker:

tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
tagcheck.OutputLevel = 4

########################################################################
#
# Standard configuration
#
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 200                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType   = "2010" 
DaVinci().Simulation = True

DaVinci().DDDBtag    = "head-20100119"
DaVinci().CondDBtag  = "sim-20100222-vc-md100"

########################################################################

DaVinci().MoniSequence = [  cheatsel,
                            tag,
#                            tagcheck
                         ]  # The algorithms

########################################################################
#bsjpsiphi
EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00006522/0000/00006522_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

#EventSelector().Input = [
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008916/0000/00008916_00000083_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    ]
        

