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
tag.addTool( TriggerTisTos )
tag.TriggerTisTos.OutputLevel = 4
tag.addTool( MCMatchObjP2MCRelator )
tag.MCMatchObjP2MCRelator.OutputLevel = 4
tag.addTool( BackgroundCategory )
tag.BackgroundCategory.OutputLevel = 4


tag.addTool( BTaggingTool )
tag.BTaggingTool.ChoosePVCriterium = "PVbyIP"#needed by CheatedSel
tag.BTaggingTool.CombineTaggersName = "CombineTaggersNN"#combine taggers with NN
tag.BTaggingTool.OutputLevel    = 3
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
tagana.RequireTisTos = True

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

#################################################################
# Triger information
from Configurables import L0Conf
#See L0Conf().TCK at $L0TCKROOT/options/L0DUConfig_201002_0x1210-0x1810.opts
L0Conf().TCK = "0xDC09"

DaVinci().ReplaceL0BanksWithEmulated = True    # Redo L0

from Configurables import HltConf
DaVinci().Hlt = True
DaVinci().HltThresholdSettings = 'Physics_10000Vis_1000L0_40Hlt1_Apr09'


########################################################################
# Standard configuration
MessageSvc().Format  = "% F%30W%S%7W%R%T %0W%M"

DaVinci().EvtMax     = 200                         # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 10
DaVinci().TupleFile     = "analysis.root"     # Ntuple
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file

DaVinci().Simulation = True
DaVinci().DataType   = "2010"
DaVinci().DDDBtag    = "head-20100119"
DaVinci().CondDBtag  = "sim-20100222-vc-md100"

DaVinci().MoniSequence = [ cheatsel,
                           tag,
                           #tagcheck,
                           tagana
                           ]  # The algorithms

########################################################################
#Bs2DsPi
EventSelector().Input = [ "DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/2010/DST/00005967/0000/00005967_00000001_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'",
                          "DATAFILE='castor://castorlhcb.cern.ch:9002//castor/cern.ch/grid/lhcb/MC/2010/DST/00005967/0000/00005967_00000002_1.dst?svcClass=lhcbdata&castorVersion=2' TYP='POOL_ROOTTREE' OPT='READ'" ]


