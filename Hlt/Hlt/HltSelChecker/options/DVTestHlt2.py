### $Id: DVTestHlt2.py,v 1.8 2010-05-10 12:49:36 pkoppenb Exp $
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
DaVinci().EvtMax = 10000                   # Number of events  # 400000
DaVinci().PrintFreq = 100
DaVinci().SkipEvents = 0
DaVinci().DataType = "2010"                    # Default is "DC06"
DaVinci().MoniSequence = [ moni ]              # Declare moni sequence
DaVinci().Hlt = True                # Both Hlt levels
DaVinci().HltThresholdSettings = 'Physics_25Vis_25L0_2Hlt1_2Hlt2_Apr10'
DaVinci().HistogramFile = "DVTestHlt.root"     # Histogram file
DaVinci().TupleFile = "HLT-MB.root"     # Histogram file
DaVinci().ReplaceL0BanksWithEmulated = True 

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# This will speed things up, but don't do that if you add a DaVinci selection

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []
    from Configurables import DeterministicPrescaler, HltCorrelations
    DeterministicPrescaler("Hlt2TransparentPreScaler").AcceptFraction = 0.
    HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "Hlt1GlobalPass" ]
appendPostConfigAction(noInit)

DaVinci().Input = [ "   DATAFILE='PFN:castor:/castor/cern.ch/grid/lhcb/data/2010/DST/00006335/0000/00006335_00003897_4.MiniBias.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
