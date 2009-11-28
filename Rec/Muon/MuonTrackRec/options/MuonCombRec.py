# MuonCombRec tool default specific options
from Configurables import MuonCombRec
#muonComb = MuonCombRec(OutputLevel = DEBUG)
muonComb = MuonCombRec(OutputLevel = INFO)
#                 R1   R2   R3   R4
muonComb.xFOIs = [100.,200.,300.,400.,  # M1
                  100.,200.,300.,400.,  # M2
                  100.,200.,300.,400.,  # M3
                  400.,400.,400.,400.]  # M4

#                 R1   R2   R3   R4
muonComb.yFOIs = [ 30., 60.,100.,150.,  # M1
                   60.,120.,180.,240.,  # M2
                   60.,120.,240.,480.,  # M3
                   60.,120.,240.,480.]  # M4

muonComb.MeasureTime = True
muonComb.CloneKiller = True
muonComb.SkipStation = -1 # -1=no skip, 0=M1, 1=M2, 2=M3, 3=M4, 4=M5
muonComb.BField = False
muonComb.DecodingTool = "MuonHitDecode"
muonComb.PadRecTool = "MuonPadRec"
muonComb.ClusterTool = "MuonFakeClustering" # to enable: "MuonClusterRec"

muonComb.PhysicsTiming = True
muonComb.AssumeCosmics = False
muonComb.AssumePhysics = True
muonComb.AddXTalk = True # has no effect if "MuonClusterRec" is selected
muonComb.SeedStation = 4 # default seet station is M5

