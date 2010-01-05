########################################################################
# $Id: DVTutorial_1a.py,v 1.1 2010-01-05 16:36:41 jdickens Exp $
#
# Options for exercise 1
#
# @author Jeremy Dickens
# @date 2009-12-21
#
########################################################################
from Gaudi.Configuration import *

#######################################################################
#######################################################################
#
# Filter muons and plot some info
#
#######################################################################
#######################################################################

from Configurables import GaudiSequencer

TutorialSeq = GaudiSequencer("TutorialSeq")
TutorialSeq.IgnoreFilterPassed = False

#######################################################################
# Create filter desktop
#######################################################################

from Configurables import FilterDesktop

FilterMuonsName = "FilterMuons"
FilterMuons = FilterDesktop(FilterMuonsName)
FilterMuons.OutputLevel = 4

FilterMuons.InputLocations = [ "Phys/StdLooseMuons" ]
FilterMuons.Code = "ALL"
#FilterMuons.Code = "(MIPCHI2DV(PRIMARY) > 4)"

FilterMuonsOutput = "Phys/"
FilterMuonsOutput += FilterMuonsName

#######################################################################
# Add some plots
#######################################################################

from Configurables import LoKi__Hybrid__PlotTool as PlotTool

MuonPlots = PlotTool("MuonPlots")
FilterMuons.addTool(MuonPlots) # make it privately owned by FilterMuons

# configure
FilterMuons.MuonPlots.OutputLevel = 4
FilterMuons.MuonPlots.Histos = { "P/GeV"  : ('Muon_P_GeV', 0, 50),
                                 "PT/GeV" : ('Muon_PT_GeV', 0, 5),
                                 "MIPDV(PRIMARY)"      : ('Muon_IP', 0, 10),
                                 "MIPCHI2DV(PRIMARY)"  : ('Muon_IPCHI2', 0, 10)
                                 }

FilterMuons.HistoProduce = True
FilterMuons.OutputPlotsTool = MuonPlots # plot output particles
FilterMuons.OutputPlotsPath = "FilterMuons" # set directory name

TutorialSeq.Members += [FilterMuons]


#######################################################################
#######################################################################
#
# Define some monitoring tools
#
#######################################################################
#######################################################################

MonitoringSeq = GaudiSequencer("MonitoringSeq")
MonitoringSeq.IgnoreFilterPassed = False

########################################################################
# Check have filtered at least one muon
########################################################################

from Configurables import CheckSelResult

MuonCheckResult = CheckSelResult("MuonCheckSelResult")
MuonCheckResult.Algorithms = [ FilterMuonsOutput ]
MuonCheckResult.OutputLevel = 3

MonitoringSeq.Members += [ MuonCheckResult ]

########################################################################
# Print a header
########################################################################

from Configurables import PrintHeader

MuonsCheckHeader = PrintHeader( "PassedMuonsCheck" )
MuonsCheckHeader.OutputLevel = 6

MonitoringSeq.Members += [ MuonsCheckHeader ]


########################################################################
########################################################################
#
# Standard configuration
#
########################################################################
########################################################################

from Configurables import DaVinci

DaVinci().EvtMax = 1000                        # Number of events
DaVinci().SkipEvents = 0                       # Events to skip
DaVinci().PrintFreq = 1                        # Events to skip
DaVinci().DataType = "2009"                    # Default is "MC06"
DaVinci().Simulation   = False
DaVinci().HistogramFile = "DVHistos_1.root"    # Histogram file
DaVinci().TupleFile = "DVNtuples.root"         # Ntuple
DaVinci().UserAlgorithms = [ TutorialSeq ]     # The algorithms
DaVinci().MoniSequence = [ MonitoringSeq ]     # Monitoring
# DaVinci().MainOptions  = "" # None
########################################################################

