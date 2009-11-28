#
# use  MuonNNetRec as Track rec tool
#YourAlgorithm.addTool(muonNNet)

from Configurables import MuonNNetRec

muonNNet = MuonNNetRec(OutputLevel = INFO)
muonNNet.ClusterTool = "MuonClusterRec"
