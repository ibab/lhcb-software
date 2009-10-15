########################################################################
#
# Options for BTaggingAnalysis algorithm
#
# @author Marco Musy
# @date 2009-07-05
#
########################################################################
from Gaudi.Configuration import *
from Configurables import GaudiSequencer

# If you want to import .opts options, do this first
importOptions("$STDOPTS/PreloadUnits.opts")

########################################################################
#
# Some selection. 
#
from Configurables import BTaggingTool, BTaggingAnalysis, CheatedSelection

# The CheatedSelection:
preselSeq = CheatedSelection("CheatedSelection")
preselSeq.AssociatorInputData = [ "Phys/TaggingPions" ]

# Some Real Selection:
#importOptions( "$FLAVOURTAGGINGOPTS/DC06SelBs2DsPi.opts" )
#preselSeq = GaudiSequencer("SeqDC06SelBs2DsPi")


#preselSeq.OutputLevel = 2

########################################################################
#
# BTaggingAnalysis 
#
tagana = BTaggingAnalysis("BTaggingAnalysis")
tagana.OutputLevel = 3

tagana.InputLocations = [ "Phys/CheatedB", "Phys/TaggingPions" ]

MessageSvc().Format = "% F%20W%S%7W%R%T %0W%M"
########################################################################
#
# Standard configuration
#
from Configurables import DaVinci
DaVinci().EvtMax     = 100                        # Number of events
DaVinci().SkipEvents = 0                           # Events to skip
DaVinci().PrintFreq  = 1
DaVinci().DataType   = "MC09"                  # Default is "DC06"
DaVinci().Simulation    = True
DaVinci().HistogramFile = "DVHistos.root"     # Histogram file
DaVinci().TupleFile     = "analysis.root"     # Ntuple

DaVinci().UserAlgorithms = [ preselSeq, tagana ]  # The algorithms

########################################################################
# HLT
DaVinci().ReplaceL0BanksWithEmulated = True ## enable if you want to rerun L0
#DaVinci().Hlt2IgnoreHlt1Decision = True    ## Hlt2 irrespective of Hlt1

DaVinci().Hlt = True
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Aug09'

########################################################################
# Data file BsDspi:
#EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002034/DST/0000/00002034_00000001_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

#bujpsik dc06 stripping
#EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/production/DC06/v1r0/00002033/DST/0000/00002033_00000001_2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

#bdjpsiee for DC08
#EventSelector().Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/2008/DST/00003393/0000/00003393_00000001_5.dst' TYP='POOL_ROOTTREE' OPT='READ'"]

#MC09 bsdspi
EventSelector().Input   = [ "DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/MC/MC09/DST/00004869/0000/00004869_00000001_1.dst' TYP='POOL_ROOTTREE' OPT='READ'"]
########################################################################
