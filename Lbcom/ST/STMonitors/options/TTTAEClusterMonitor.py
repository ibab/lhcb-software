from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniTTSeq = GaudiSequencer( 'MoniTTSeq' )
MoniTTSeq.MeasureTime = True
ApplicationMgr().TopAlg.append( MoniTTSeq )

from Configurables import ( ST__STTAEClusterMonitor )

ttTAEClusterMonitor = ST__STTAEClusterMonitor("TTTAEClusterMonitor")
ttTAEClusterMonitor.DetType = "TT"
ttTAEClusterMonitor.ClusterLocations = [
   "Prev7/Raw/TT/Clusters",
   "Prev6/Raw/TT/Clusters",
   "Prev5/Raw/TT/Clusters",
   "Prev4/Raw/TT/Clusters",
   "Prev3/Raw/TT/Clusters",
   "Prev2/Raw/TT/Clusters",
   "Prev1/Raw/TT/Clusters",
   "Raw/TT/Clusters",
   "Next1/Raw/TT/Clusters",
   "Next2/Raw/TT/Clusters",
   "Next3/Raw/TT/Clusters",
   "Next4/Raw/TT/Clusters",
   "Next5/Raw/TT/Clusters",
   "Next6/Raw/TT/Clusters",
   "Next7/Raw/TT/Clusters"
   ]
ttTAEClusterMonitor.ByServiceBox= True
ttTAEClusterMonitor.ByDetectorRegion = False
ttTAEClusterMonitor.OutputLevel = 3
MoniTTSeq.Members.append(ttTAEClusterMonitor)

