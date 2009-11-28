#recommended options for physics data
#
# use  MuonCombRec as Track rec tool
#YourAlgorithm.addTool(muonComb)

from Configurables import MuonCombRec

muonComb = MuonCombRec(OutputLevel = INFO)
muonComb.ClusterTool = "MuonClusterRec"
muonComb.AddXTalk = False




