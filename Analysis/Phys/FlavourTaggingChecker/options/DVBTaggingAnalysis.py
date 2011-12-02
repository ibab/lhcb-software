########################################################################
#
# Example Options for BTaggingAnalysis algorithm
#
# @author Marco Musy
# @date 2010-09-07
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer, DaVinci

########################################################################
# CheatedSelection. 

from Configurables import CheatedSelection

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.Inputs = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 4

########################################################################
# Flavour tagging. 

from Configurables import BTagging, BTaggingTool, BTaggingAnalysis, BTaggingChecker, TriggerTisTos, TaggingUtilsChecker, TaggingUtils, MCMatchObjP2MCRelator, BackgroundCategory

location = "Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.Inputs = [ location ]

tag.addTool( TriggerTisTos )
tag.TriggerTisTos.OutputLevel = 4
tag.addTool( TaggingUtilsChecker )
tag.TaggingUtilsChecker.OutputLevel = 4
tag.addTool( TaggingUtils )
tag.TaggingUtils.OutputLevel = 4
tag.addTool( MCMatchObjP2MCRelator )
tag.MCMatchObjP2MCRelator.OutputLevel = 4
tag.addTool( BackgroundCategory )
tag.BackgroundCategory.OutputLevel = 4

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP" #needed by CheatedSel
tag.BTaggingTool.UseReFitPV = False
#tag.BTaggingTool.ForceSignalID = "Bs" #"Bs", "Bu", "Bd"
#tag.BTaggingTool.CombineTaggersName = "CombineTaggersNN" #combine taggers with NN, good for mc
tag.BTaggingTool.OutputLevel = 4

#Import cuts
#from FlavourTagging.Tunings import TuneTool
#TuneTool(tag,"MC10") #for example

########################################################################
# Flavour tagging Checker:

tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.Inputs = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
tagcheck.OutputLevel = 3

########################################################################
# BTaggingAnalysis ntuple creation

tagana = BTaggingAnalysis("BTaggingAnalysis")
tagana.Inputs = [ location, 
                  "Phys/TaggingParticles"
                  ]

tagana.TagOutputLocation =  location + "/FlavourTags"

tagana.ChoosePVCriterium = "PVbyIP"  #needed by CheatedSel
tagana.BHypoCriterium = "MaximumPt"  #needed by CheatedSel
#tagana.PVReFit = False
tagana.RequireTisTos = True #TisTosTool
#tagana.SaveHlt1Lines = False #SaveHlt1Lines (require tistos)
tagana.OutputLevel = 4

tagana.addTool( TriggerTisTos )
tagana.TriggerTisTos.OutputLevel = 5
tagana.addTool( TaggingUtilsChecker )
tagana.TaggingUtilsChecker.OutputLevel = 5
tagana.addTool( MCMatchObjP2MCRelator )
tagana.MCMatchObjP2MCRelator.OutputLevel = 5
tagana.addTool( BackgroundCategory )
tagana.BackgroundCategory.OutputLevel = 5

########################################################################
# Standard configuration
MessageSvc().Format  = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 250                        # Number of events
#DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().TupleFile  = "analysis.root"     # Ntuple

DaVinci().Simulation = True
DaVinci().DataType   = "2010" 

#for bsjpsiphi 2010
DaVinci().DDDBtag    = "head-20100407" 
DaVinci().CondDBtag  = "sim-20100429-vc-md100"

DaVinci().MoniSequence = [ cheatsel,
                           tag,
                           #tagcheck,
                           tagana
                           ]  # The algorithms

########################################################################


EventSelector().Input = [
    "DATAFILE='PFN:/castor/cern.ch/grid/lhcb/MC/MC10/ALLSTREAMS.DST/00008916/0000/00008916_00000083_1.allstreams.dst' TYP='POOL_ROOTTREE' OPT='READ'"
    #jpsik+
    #"DATAFILE='PFN:/castor/cern.ch/grid/lhcb/LHCb/Collision11/DIMUON.DST/00010193/0000/00010193_00000479_1.dimuon.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    #bsjpsiphi
    #"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00006522/0000/00006522_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    #bdjpsikshort
    #"DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00007353/0000/00007353_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
    ]
