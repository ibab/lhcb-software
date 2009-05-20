#
# B->DPi selection for stripping (consider as an example). 
# This is an example of a simple wrapper over user's own algorithm. 
# No StrippingMember functionality is used here. 
#

from os import environ
from Gaudi.Configuration import *
from Configurables import DecayTreeTuple
from Configurables import CombineParticles, FilterDesktop
from Configurables import TupleToolMCBackgroundInfo, TupleToolMCTruth, BackgroundCategory
from StrippingConf.StrippingLine import StrippingLine

########################################################################
B2DhOfflineDplus2KPiPi = CombineParticles("B2DhOfflineDplus2KPiPi")
B2DhOfflineDplus2KPiPi.InputLocations = [ "StdTightPions", "StdTightKaons" ]
B2DhOfflineDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc"

B2DhOfflineDplus2KPiPi.DaughtersCuts = {"pi+" : "((PT > 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))" , "K+" :  "((PT> 300*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"};
B2DhOfflineDplus2KPiPi.CombinationCut = "( (ADAMASS('D+') < 21*MeV) & (APT > 2.*GeV) )";
B2DhOfflineDplus2KPiPi.MotherCut = "((VFASPF(VCHI2/VDOF)<15) & (BPVVDCHI2>100.) & (MIPCHI2DV(PRIMARY)>9))";
########################################################################
B2DhOfflineBachelor = FilterDesktop("B2DhOfflineBachelor")
B2DhOfflineBachelor.InputLocations  = [ "StdLoosePions" ]
B2DhOfflineBachelor.Code = "((PT > 500*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 9))"
########################################################################
B2DhOfflineSel = CombineParticles("B2DhOfflineSel")
B2DhOfflineSel.InputLocations  = [   "B2DhOfflineDplus2KPiPi",
                                                 "B2DhOfflineBachelor"]
B2DhOfflineSel.DecayDescriptor  = "[B0 -> D+ pi-]cc"
B2DhOfflineSel.DaughtersCuts = { "D+" : "ALL" }
B2DhOfflineSel.CombinationCut = "(ADAMASS('B0')<50*MeV)"
B2DhOfflineSel.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVIPCHI2()<16.) & (BPVVDCHI2 > 6.25) & (BPVDIRA>0.9999)"
########################################################################
B2DhOfflineFilterSequence = GaudiSequencer("B2DhOfflineFilterSequence")
B2DhOfflineFilterSequence.Members += [ B2DhOfflineDplus2KPiPi ]
B2DhOfflineFilterSequence.Members += [ B2DhOfflineBachelor ]
B2DhOfflineFilterSequence.Members += [ B2DhOfflineSel ]
########################################################################
########################################################################
B2DhFilterSequence = GaudiSequencer("B2DhFilterSequence")
B2DhFilterSequence.Members += [B2DhOfflineFilterSequence]

# Create the line for this selection
line = StrippingLine('B2DPi'
               , prescale = 1
               ,  algos = [ B2DhFilterSequence ]
               ,  stream = 'BExclusive'
               )
