from Gaudi.Configuration import *
from Configurables import GaudiSequencer


# Add a sequence
HltWriteSeq = GaudiSequencer("HltWriteSeq")
HltWriteSeq.MeasureTime = TRUE
ApplicationMgr().TopAlg += [ HltWriteSeq ]

from Configurables import HltDecReportsMaker,HltSelReportsMaker,HltVertexReportsMaker
HltDecReportsMaker = HltDecReportsMaker()
HltSelReportsMaker = HltSelReportsMaker()
HltVertexReportsMaker = HltVertexReportsMaker()
HltWriteSeq.Members += [ HltDecReportsMaker , HltSelReportsMaker ,HltVertexReportsMaker ]
from Configurables import HltDecReportsWriter,HltSelReportsWriter,HltVertexReportsWriter
HltDecReportsWriter = HltDecReportsWriter()
HltSelReportsWriter = HltSelReportsWriter()
HltVertexReportsWriter = HltVertexReportsWriter()
HltWriteSeq.Members += [ HltDecReportsWriter, HltSelReportsWriter,HltVertexReportsWriter ]

HltDecReportsMaker.OutputLevel            = 3
HltDecReportsWriter.OutputLevel            = 3
HltSelReportsMaker.OutputLevel            = 3
HltSelReportsWriter.OutputLevel            = 3
HltVertexReportsMaker.OutputLevel            = 3
HltVertexReportsWriter.OutputLevel            = 3


# 0=infinity 1=always 100=every 100th event etc.
HltDecReportsMaker.DebugEventPeriod = 0
# default info levels  0=no output 1=if passed 2=if errors 3=if passed or errors 4=always
HltDecReportsMaker.InfoLevelDecision = 4
HltDecReportsMaker.InfoLevelDecisionDebug = 4
HltDecReportsMaker.InfoLevelNonDecision = 0
HltDecReportsMaker.InfoLevelNonDecisionDebug = 3 


# 0=infinity 1=always 100=every 100th event etc.
HltSelReportsMaker.DebugEventPeriod = 0
# default info levels  0=lhcbids 1=lhcbids+standard info 2=lhcbids+extra info 3=lhcbids+standard info+extra info
HltSelReportsMaker.InfoLevelDecision = 3
HltSelReportsMaker.InfoLevelDecisionDebug = 3
HltSelReportsMaker.InfoLevelNonDecision = 0
HltSelReportsMaker.InfoLevelNonDecisionDebug = 3 
#     global enablers
HltSelReportsMaker.InfoLevelTrack=3
HltSelReportsMaker.InfoLevelTrackDebug=3
HltSelReportsMaker.InfoLevelRecVertex=3
HltSelReportsMaker.InfoLevelRecVertexDebug=3
HltSelReportsMaker.InfoLevelParticle=3
HltSelReportsMaker.InfoLevelParticleDebug=3
HltSelReportsMaker.InfoLevelCaloCluster=3
HltSelReportsMaker.InfoLevelCaloClusterDebug=3

# maximal number of candidates to save (0=none)
HltSelReportsMaker.MaxCandidatesDecision =100
HltSelReportsMaker.MaxCandidatesDecisionDebug =500
HltSelReportsMaker.MaxCandidatesNonDecision =0
HltSelReportsMaker.MaxCandidatesNonDecisionDebug =500

# selection specific setting
HltSelReportsMaker.SelectionMaxCandidates = { 
   "PV2D"   : 0, 
   "RZVelo" : 0, 
   "Velo"   : 0 
}
HltSelReportsMaker.SelectionMaxCandidatesDebug = { 
   "PV2D"   : 0, 
   "RZVelo" : 0, 
   "Velo"   : 0 
}

HltVertexReportsMaker.VertexSelections += ["PV2D"]


