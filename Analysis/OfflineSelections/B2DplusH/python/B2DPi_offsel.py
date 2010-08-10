########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple, PhysDesktop
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
########################################################################
B2DPiOfflineDplus2KPiPi = CombineParticles("B2DPiOfflineDplus2KPiPi")
B2DPiOfflineDplus2KPiPi.InputLocations = [ "StdTightPions", "StdNoPIDsKaons" ]
B2DPiOfflineDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc"

B2DPiOfflineDplus2KPiPi.DaughtersCuts = {"pi+" : "((PT > 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))" , "K+" :  "((PT> 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"};
B2DPiOfflineDplus2KPiPi.CombinationCut = "( (ADAMASS('D+') < 25*MeV) & (APT > 2.*GeV) )";
B2DPiOfflineDplus2KPiPi.MotherCut = "((VFASPF(VCHI2/VDOF)<15) & (BPVVDCHI2>100.) & (MIPCHI2DV(PRIMARY)>9))";
########################################################################
B2DPiOfflineBachelor = FilterDesktop("B2DPiOfflineBachelor")
B2DPiOfflineBachelor.InputLocations  = [ "StdLoosePions" ]
B2DPiOfflineBachelor.Code = "((PT > 500*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"
########################################################################
B2DPiOfflineSel = CombineParticles("B2DPiOfflineSel")
B2DPiOfflineSel.InputLocations  = [   "B2DPiOfflineDplus2KPiPi",
                                                 "B2DPiOfflineBachelor"]
B2DPiOfflineSel.DecayDescriptor  = "[B0 -> D+ pi-]cc"
B2DPiOfflineSel.DaughtersCuts = { "D+" : "ALL" }
B2DPiOfflineSel.CombinationCut = "(ADAMASS('B0')<50*MeV)"
B2DPiOfflineSel.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<16.) & (BPVVDCHI2 > 6.25) & (BPVDIRA>0.9999)"
########################################################################
B2DPiOfflineFilterSequence = GaudiSequencer("B2DPiOfflineFilterSequence")
B2DPiOfflineFilterSequence.Members += [ B2DPiOfflineDplus2KPiPi ]
B2DPiOfflineFilterSequence.Members += [ B2DPiOfflineBachelor ]
B2DPiOfflineFilterSequence.Members += [ B2DPiOfflineSel ]
########################################################################
