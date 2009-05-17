from Gaudi.Configuration import *
# Job options to configure the data on demand service for decoding the RawEvent

MoniITSeq = GaudiSequencer( 'MoniITSeq' )
MoniITSeq.MeasureTime = True
#ApplicationMgr().TopAlg.append( MoniITSeq )

from Configurables import ( ST__STTAEClusterMonitor )

itTAEClusterMonitor = ST__STTAEClusterMonitor("ITTAEClusterMonitor")
itTAEClusterMonitor.DetType = "IT"
itTAEClusterMonitor.MaxClusterSize= 4
itTAEClusterMonitor.ByServiceBox= True
itTAEClusterMonitor.ClusterLocations = [
   "Prev7/Raw/IT/Clusters",
   "Prev6/Raw/IT/Clusters",
   "Prev5/Raw/IT/Clusters",
   "Prev4/Raw/IT/Clusters",
   "Prev3/Raw/IT/Clusters",
   "Prev2/Raw/IT/Clusters",
   "Prev1/Raw/IT/Clusters",
   "Raw/IT/Clusters",
   "Next1/Raw/IT/Clusters",
   "Next2/Raw/IT/Clusters",
   "Next3/Raw/IT/Clusters",
   "Next4/Raw/IT/Clusters",
   "Next5/Raw/IT/Clusters",
   "Next6/Raw/IT/Clusters",
   "Next7/Raw/IT/Clusters"
   ]
itTAEClusterMonitor.ByDetectorRegion = False
itTAEClusterMonitor.OutputLevel = 3
MoniITSeq.Members.append(itTAEClusterMonitor)

