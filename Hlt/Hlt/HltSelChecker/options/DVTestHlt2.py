### $Id: DVTestHlt2.py,v 1.3 2009-11-11 10:15:09 pkoppenb Exp $
#
#   Hlt 2 selections test job
#
#   @author P. Koppenburg
#   @date 2008-08-04
# /
from Gaudi.Configuration import *
##################################################################
# 
# Define a generic Hlt2 monitor sequence
#
from Configurables import GaudiSequencer
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
##################################################################
# 
# to get Hlt2 correlations
#
importOptions( "$HLTSELCHECKERROOT/options/Hlt2Correlations.py")
##################################################################
# 
# to get Hlt2 monitor plots
#
# importOptions( "$HLTSELCHECKERROOT/options/Hlt2MonitorPlots.py")
##################################################################
# 
# Uncomment to Read HltSummary and printout candidates
#
# from Configurables import ReadHltReport
# moni.Members += [ ReadHltReport() ]
# ReadHltReport().PrintParticles = True
#################################################################
#
# DaVinci
#
from Configurables import DaVinci
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().PrintFreq = 1
DaVinci().DataType = "MC09"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().MoniSequence = [ moni ]              # Declare moni sequence
DaVinci().Hlt = True                # Both Hlt levels
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Aug09'
# DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().HistogramFile = "DVTestHlt.root"     # Histogram file

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# from Configurables import HltConf
# HltConf().AdditionalHlt1Lines = ['Hlt1BeamGasCrossingForcedRZReco']
# HltConf().AdditionalHlt2Lines = ['Hlt2Bs2PhiGamma', 'Hlt2Bd2KstGamma']

# This will speed things up, but don't do that if you add a DaVinci selection

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []

appendPostConfigAction(noInit)

