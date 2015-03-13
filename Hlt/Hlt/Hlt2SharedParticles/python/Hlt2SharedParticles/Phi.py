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
from HltLine.HltLine import bindMembers, Hlt2Member
from Hlt2SharedParticles.TrackFittedBasicParticles import  BiKalmanFittedRichKaons
__all__ = ( 'Phi2KK', 'UnbiasedPhi2KK' )

Hlt2SharedPhi2KK = Hlt2Member( CombineParticles
                               , 'Phi2KK'
                               , Inputs = [ BiKalmanFittedRichKaons ]
                               , DecayDescriptor = "phi(1020) -> K+ K-"
                               , DaughtersCuts = { "K+" : "ALL"}
                               , CombinationCut = "(ADAMASS('phi(1020)')<50*MeV)"
                               , MotherCut = "(VFASPF(VCHI2PDOF)<25)" )

Phi2KK = bindMembers( 'Shared', [ BiKalmanFittedRichKaons, Hlt2SharedPhi2KK ] )

### 
#
#  Hlt2 Phi for Bs -> Phi Phi selection
#
#  @author N.Styles n.a.styles@ed.ac.uk
#  @date 2008-10-09
#
##

Hlt2SharedUnbiasedPhi2KK = Hlt2Member( CombineParticles
                                       , "UnbiasedPhi2KK"
                                       , Inputs = [ BiKalmanFittedRichKaons ]
                                       , DecayDescriptor = "phi(1020) -> K+ K-"
                                       , DaughtersCuts = { "K+" : "(PT > 500*MeV) & (PIDK>0.)" }
                                       , CombinationCut = "(ADAMASS('phi(1020)')<30*MeV)"
                                       , MotherCut = "(VFASPF(VCHI2PDOF)<25)" )

UnbiasedPhi2KK = bindMembers( 'Shared', [ BiKalmanFittedRichKaons, Hlt2SharedUnbiasedPhi2KK ] )
