########################################################################
#
# Example Options for BTaggingAnalysis algorithm
#
# @author Marco Musy
# @date 2010-02-11
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
cheatsel.OutputLevel = 3

########################################################################
# Flavour tagging. 

from Configurables import BTagging, BTaggingTool, BTaggingAnalysis, BTaggingChecker, TriggerTisTos, TaggingUtilsChecker, TaggingUtils, MCMatchObjP2MCRelator, BackgroundCategory

location = "Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
tag.OutputLevel    = 4
tag.addTool( PhysDesktop )
tag.PhysDesktop.OutputLevel = 4

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP"#needed by CheatedSel
#tag.BTaggingTool.ChoosePVCriterium = "RefitPV"
#tag.BTaggingTool.UseReFitPV = True

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
                          "Phys/TaggingElectrons",
                          "Phys/TaggingMuons",
                          "Phys/TaggingPions" 
                          ]
tagana.TagOutputLocation =  location + "/FlavourTags"

tagana.ChoosePVCriterium = "PVbyIP"  #needed by CheatedSel   
tagana.RequireTisTos = False

tagana.OutputLevel = 4

tagana.addTool( PhysDesktop )
tagana.PhysDesktop.OutputLevel = 4
tagana.addTool( TriggerTisTos )
tagana.TriggerTisTos.OutputLevel = 4
tagana.addTool( TaggingUtilsChecker )
tagana.TaggingUtilsChecker.OutputLevel = 4
tagana.addTool( MCMatchObjP2MCRelator )
tagana.MCMatchObjP2MCRelator.OutputLevel = 4
tagana.addTool( BackgroundCategory )
tagana.BackgroundCategory.OutputLevel = 4


########################################################################
# Standard configuration
MessageSvc().Format  = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 200                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 10
DaVinci().TupleFile     = "analysis.root"     # Ntuple
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file

DaVinci().Simulation = True
DaVinci().DataType   = "MC09" 

DaVinci().MoniSequence = [ cheatsel,
                           tag,
                           #tagcheck,
                           tagana
                           ]  # The algorithms

########################################################################
#DAVINCI/DAVINCI_HEAD/DaVinciSys/tests/options/DVTestTagging.py
# example data files
#bsdspi_1.py
EventSelector().Input   = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00005138/0000/00005138_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

#bdjpsiks_nu3
#EventSelector().Input   = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00005146/0000/00005146_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

