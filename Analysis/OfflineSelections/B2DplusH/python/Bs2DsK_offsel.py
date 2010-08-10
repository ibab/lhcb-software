########################################################################
from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple, PhysDesktop
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
########################################################################
Bs2DsKOfflineDs2KKPi = CombineParticles("Bs2DsKOfflineDs2KKPi")
Bs2DsKOfflineDs2KKPi.InputLocations = [ "StdTightPions", "StdTightKaons" ]
Bs2DsKOfflineDs2KKPi.DecayDescriptor = "[D_s+ -> K- K+ pi+]cc"

Bs2DsKOfflineDs2KKPi.DaughtersCuts = {"pi+" : "((PT > 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))" , "K+" :  "((PT> 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9) & ((PIDK - PIDp) > -10) )"};
Bs2DsKOfflineDs2KKPi.CombinationCut = "( (ADAMASS('D_s+') < 25*MeV) & (APT > 2.*GeV) )";
Bs2DsKOfflineDs2KKPi.MotherCut = "((VFASPF(VCHI2/VDOF)<15) & (BPVVDCHI2>100.) & (MIPCHI2DV(PRIMARY)>9))";
########################################################################
Bs2DsKOfflineBachelor = FilterDesktop("Bs2DsKOfflineBachelor")
Bs2DsKOfflineBachelor.InputLocations  = [ "StdTightKaons" ]
Bs2DsKOfflineBachelor.Code = "((PT > 500*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9) & ((PIDK - PIDp) > 5) & (PIDK > 4))"
########################################################################
Bs2DsKOfflineSel = CombineParticles("Bs2DsKOfflineSel")
Bs2DsKOfflineSel.InputLocations  = [   "Bs2DsKOfflineDs2KKPi",
                                                 "Bs2DsKOfflineBachelor"]
Bs2DsKOfflineSel.DecayDescriptor  = "[B_s0 -> D_s+ K-]cc"
Bs2DsKOfflineSel.DaughtersCuts = { "D+" : "ALL" }
Bs2DsKOfflineSel.CombinationCut = "(ADAMASS('B_s0')<50*MeV)"
Bs2DsKOfflineSel.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<16.) & (BPVVDCHI2 > 144) & (BPVDIRA>0.9999)"
########################################################################
Bs2DsKOfflineFilterSequence = GaudiSequencer("Bs2DsKOfflineFilterSequence")
Bs2DsKOfflineFilterSequence.Members += [ Bs2DsKOfflineDs2KKPi ]
Bs2DsKOfflineFilterSequence.Members += [ Bs2DsKOfflineBachelor ]
Bs2DsKOfflineFilterSequence.Members += [ Bs2DsKOfflineSel ]
########################################################################
