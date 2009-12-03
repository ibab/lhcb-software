### $Id: DVTestHlt2.py,v 1.7 2009-12-03 12:23:23 pkoppenb Exp $
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
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py")
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
from Configurables import DaVinci, HltConf
DaVinci().EvtMax = 400                   # Number of events  # 400000
DaVinci().PrintFreq = 1000
DaVinci().SkipEvents = 0
DaVinci().DataType = "MC09"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().MoniSequence = [ moni ]              # Declare moni sequence
DaVinci().Hlt = True                # Both Hlt levels
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Hlt2_Oct09'
# DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().HistogramFile = "DVTestHlt.root"     # Histogram file
DaVinci().TupleFile = "HLT-MB.root"     # Histogram file
# DaVinci().ReplaceL0BanksWithEmulated = True 

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# This will speed things up, but don't do that if you add a DaVinci selection

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []
    from Configurables import DeterministicPrescaler, HltCorrelations
    DeterministicPrescaler("Hlt2TransparentPreScaler").AcceptFraction = 0.
    HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "Hlt1GlobalPass" ]
appendPostConfigAction(noInit)

