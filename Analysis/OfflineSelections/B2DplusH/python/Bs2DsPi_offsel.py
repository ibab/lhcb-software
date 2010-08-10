########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple, PhysDesktop
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
########################################################################
Bs2DsPiOfflineDs2KKPi = CombineParticles("Bs2DsPiOfflineDs2KKPi")
Bs2DsPiOfflineDs2KKPi.InputLocations = [ "StdTightPions", "StdTightKaons" ]
Bs2DsPiOfflineDs2KKPi.DecayDescriptor = "[D_s+ -> K- K+ pi+]cc"

Bs2DsPiOfflineDs2KKPi.DaughtersCuts = {"pi+" : "((PT > 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))" , "K+" :  "((PT> 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"};
Bs2DsPiOfflineDs2KKPi.CombinationCut = "( (ADAMASS('D_s+') < 25*MeV) & (APT > 2.*GeV) )";
Bs2DsPiOfflineDs2KKPi.MotherCut = "((VFASPF(VCHI2/VDOF)<15) & (BPVVDCHI2>100.) & (MIPCHI2DV(PRIMARY)>9))";
########################################################################
Bs2DsPiOfflineBachelor = FilterDesktop("Bs2DsPiOfflineBachelor")
Bs2DsPiOfflineBachelor.InputLocations  = [ "StdTightPions" ]
Bs2DsPiOfflineBachelor.Code = "((PT > 500*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"
########################################################################
Bs2DsPiOfflineSel = CombineParticles("Bs2DsPiOfflineSel")
Bs2DsPiOfflineSel.InputLocations  = ["Bs2DsPiOfflineDs2KKPi","Bs2DsPiOfflineBachelor"]
Bs2DsPiOfflineSel.DecayDescriptor  = "[B_s0 -> D_s+ pi-]cc"
Bs2DsPiOfflineSel.DaughtersCuts = { "D_s+" : "ALL" }
Bs2DsPiOfflineSel.CombinationCut = "(ADAMASS('B_s0')<50*MeV)"
Bs2DsPiOfflineSel.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<16.) & (BPVVDCHI2 > 64) & (BPVDIRA>0.9999)"
########################################################################
Bs2DsPiOfflineFilterSequence = GaudiSequencer("Bs2DsPiOfflineFilterSequence")
Bs2DsPiOfflineFilterSequence.Members += [ Bs2DsPiOfflineDs2KKPi ]
Bs2DsPiOfflineFilterSequence.Members += [ Bs2DsPiOfflineBachelor ]
Bs2DsPiOfflineFilterSequence.Members += [ Bs2DsPiOfflineSel ]
