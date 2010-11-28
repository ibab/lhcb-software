### @file
#
#  Standard Phi
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers
from Hlt2SharedParticles.TrackFittedBasicParticles import  BiKalmanFittedKaons
__all__ = ( 'Phi2KK', 'UnbiasedPhi2KK' )

Hlt2SharedPhi2KK = CombineParticles("Hlt2SharedPhi2KK")
Hlt2SharedPhi2KK.InputLocations = [ BiKalmanFittedKaons.outputSelection() ]
Hlt2SharedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 

Hlt2SharedPhi2KK.DaughtersCuts = { "K+" : "ALL"} 
Hlt2SharedPhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<50*MeV)"
Hlt2SharedPhi2KK.MotherCut = "(VFASPF(VCHI2PDOF)<25)"

Phi2KK = bindMembers( None, [ BiKalmanFittedKaons, Hlt2SharedPhi2KK ] )

### 
#
#  Hlt2 Phi for Bs -> Phi Phi selection
#
#  @author N.Styles n.a.styles@ed.ac.uk
#  @date 2008-10-09
#
##
Hlt2SharedUnbiasedPhi2KK = CombineParticles("Hlt2SharedUnbiasedPhi2KK")
Hlt2SharedUnbiasedPhi2KK.InputLocations = [ BiKalmanFittedKaons.outputSelection() ]
Hlt2SharedUnbiasedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 
Hlt2SharedUnbiasedPhi2KK.DaughtersCuts = { "K+" : "(PT>500)" } 
Hlt2SharedUnbiasedPhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<20*MeV)"
Hlt2SharedUnbiasedPhi2KK.MotherCut = "(VFASPF(VCHI2PDOF)<25)"

UnbiasedPhi2KK = bindMembers( None, [ BiKalmanFittedKaons, Hlt2SharedUnbiasedPhi2KK ] )
