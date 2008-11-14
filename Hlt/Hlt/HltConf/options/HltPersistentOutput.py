from Gaudi.Configuration import *
from Configurables import GaudiSequencer

# Add a sequence
HltPersistentOutputSeq = GaudiSequencer("HltPersistentOutputSeq")
HltPersistentOutputSeq.MeasureTime = TRUE
GaudiSequencer("Hlt").Members += [ HltPersistentOutputSeq ]

from Configurables import HltDecReportsMaker,HltSelReportsMaker,HltVertexReportsMaker
HltDecReportsMaker = HltDecReportsMaker()
HltSelReportsMaker = HltSelReportsMaker()
HltVertexReportsMaker = HltVertexReportsMaker()
HltPersistentOutputSeq.Members += [ HltDecReportsMaker , HltSelReportsMaker ,HltVertexReportsMaker ]
from Configurables import HltDecReportsWriter,HltSelReportsWriter,HltVertexReportsWriter
HltDecReportsWriter = HltDecReportsWriter()
HltSelReportsWriter = HltSelReportsWriter()
HltVertexReportsWriter = HltVertexReportsWriter()
HltPersistentOutputSeq.Members += [ HltDecReportsWriter, HltSelReportsWriter,HltVertexReportsWriter ]

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


# selection specific settings:
#      kill large Track and Vertex Containers
HltSelReportsMaker.SelectionMaxCandidates = { 
   "PV2D"   : 0, 
   "RZVelo" : 0, 
   "Velo"   : 0,
   "Velo1"  : 0,
   "Forward1" : 0
}
HltSelReportsMaker.SelectionMaxCandidatesDebug = { 
   "PV2D"   : 0, 
   "RZVelo" : 0, 
   "Velo"   : 0, 
   "Velo1"  : 0,
   "Forward1" : 0
}

HltVertexReportsMaker.VertexSelections += ["PV2D"]


