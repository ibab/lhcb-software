########################################################################
#
# Example Options for BTagging algorithm
#
# @author Marco Musy
# @date 2009-11-30
#
########################################################################
from Gaudi.Configuration import *

########################################################################
# CheatedSelection. 

from Configurables import CheatedSelection, PhysDesktop

cheatsel = CheatedSelection("CheatedSelection")
cheatsel.InputLocations = [ "Phys/TaggingPions" ]
cheatsel.AssociatorInputData = [ "Phys/CheatedSelection/Particles" ]
cheatsel.OutputLevel = 3

########################################################################
# Flavour tagging. 

from Configurables import BTagging, BTaggingTool, BTaggingAnalysis, BTaggingChecker, TriggerTisTos, TaggingUtilsChecker, MCMatchObjP2MCRelator, BackgroundCategory

location = "Phys/CheatedSelection"

tag = BTagging("BTagging")
tag.InputLocations = [ location ]
tag.OutputLevel = 3
tag.addTool( PhysDesktop )
tag.PhysDesktop.OutputLevel = 4

tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "ChoosePVbyIP" #only needed by CheatedSel

########################################################################
# Flavour tagging Checker:

tagcheck = BTaggingChecker("BTaggingChecker")
tagcheck.InputLocations = [ location ]
tagcheck.TagsLocation = location+"/FlavourTags"
tagcheck.OutputLevel = 3

########################################################################
# BTaggingAnalysis ntuple creation

tagana = BTaggingAnalysis("BTaggingAnalysis")
tagana.InputLocations = [ location , "Phys/TaggingPions" ]
tagana.TagOutputLocation =  location + "/FlavourTags"

tagana.ChoosePVCriterium = "ChoosePVbyIP"  #only needed by CheatedSel   
tagana.RequireTisTos = False

tagana.OutputLevel = 2

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

from Configurables import DaVinci
DaVinci().EvtMax = 100                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().TupleFile     = "analysis.root"     # Ntuple
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file

DaVinci().Simulation = True
DaVinci().DataType   = "MC09" 

DaVinci().MoniSequence = [ cheatsel,
                           tag,
                           tagcheck,
                           tagana
                         ]  # The algorithms

########################################################################
# example data file
#DAVINCI/DAVINCI_HEAD/DaVinciSys/tests/options/DVTestTagging.py

#bsdspi_1.py
EventSelector().Input   = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00005138/0000/00005138_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

#bdjpsiks_nu3
#EventSelector().Input   = ["DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00005146/0000/00005146_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

