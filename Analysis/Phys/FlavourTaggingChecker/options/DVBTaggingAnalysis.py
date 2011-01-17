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

from Configurables import CheatedSelection, PhysDesktop

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.InputLocations = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 4

########################################################################
# Flavour tagging. 

from Configurables import BTagging, BTaggingTool, BTaggingAnalysis, BTaggingChecker, TriggerTisTos, TaggingUtilsChecker, TaggingUtils, MCMatchObjP2MCRelator, BackgroundCategory

location = "Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]

tag.addTool( PhysDesktop )
tag.PhysDesktop.OutputLevel = 4
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
#tag.BTaggingTool.ForceSignalID = "Bs" #"Bs", "Bu", "Bd"
#tag.BTaggingTool.CombineTaggersName = "CombineTaggersNN" #combine taggers with NN, good for mc
tag.BTaggingTool.OutputLevel = 4

#Import cuts for strip12/strip10/strip9/mc2010
#importOptions('$FLAVOURTAGGINGOPTS/cuts_strip12_v1.py')
#importOptions('$FLAVOURTAGGINGOPTS/cuts_strip12.py')
#importOptions('$FLAVOURTAGGINGOPTS/cuts_strip10_v1.py')
#importOptions('$FLAVOURTAGGINGOPTS/cuts_strip10.py')
#importOptions('$FLAVOURTAGGINGOPTS/cuts_strip9.py')
#importOptions('$FLAVOURTAGGINGOPTS/cuts_mc2010.py')

########################################################################
# Flavour tagging Checker:

tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
tagcheck.OutputLevel = 3

########################################################################
# BTaggingAnalysis ntuple creation

tagana = BTaggingAnalysis("BTaggingAnalysis")
tagana.InputLocations = [ location, 
                          #"Phys/TaggingElectrons", 
                          #"Phys/TaggingMuons",
                          #"Phys/TaggingPions"
                          "Phys/TaggingParticles"
                          ]

tagana.TagOutputLocation =  location + "/FlavourTags"

tagana.ChoosePVCriterium = "PVbyIP"  #needed by CheatedSel
tagana.BHypoCriterium = "MaximumPt"  #needed by CheatedSel
tagana.RequireTisTos = True #TisTosTool
#tagana.SaveHlt1Lines = False #SaveHlt1Lines (require tistos)
tagana.OutputLevel = 4

tagana.addTool( PhysDesktop )
tagana.PhysDesktop.OutputLevel = 5
tagana.addTool( TriggerTisTos )
tagana.TriggerTisTos.OutputLevel = 5
tagana.addTool( TaggingUtilsChecker )
tagana.TaggingUtilsChecker.OutputLevel = 5
tagana.addTool( MCMatchObjP2MCRelator )
tagana.MCMatchObjP2MCRelator.OutputLevel = 5
tagana.addTool( BackgroundCategory )
tagana.BackgroundCategory.OutputLevel = 5

#################################################################
# Triger information
from Configurables import L0Conf
L0Conf().TCK = "0xDC09"

DaVinci().ReplaceL0BanksWithEmulated = True    # Redo L0

from Configurables import HltConf
DaVinci().Hlt = False

########################################################################
# Standard configuration
MessageSvc().Format  = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 300                        # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
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
#DAVINCI/DAVINCI_HEAD/DaVinciSys/tests/options/DVTestTagging.py
# example data files

#bsjpsiphi
EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00006522/0000/00006522_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

#bdjpsi kshort
#EventSelector().Input  = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2010/DST/00007353/0000/00007353_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
