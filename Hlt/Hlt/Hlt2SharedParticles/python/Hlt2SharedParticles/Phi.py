### @file
#
#  Standard Phi
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from Hlt2SharedParticles.GoodParticles import GoodKaons
from HltConf.HltLine import bindMembers

__all__ = ( 'Phi2KK', 'UnbiasedPhi2KK' )

Hlt2SharedPhi2KK = CombineParticles("Hlt2SharedPhi2KK")
Hlt2SharedPhi2KK.addTool(PhysDesktop)
Hlt2SharedPhi2KK.PhysDesktop.InputLocations = [ "Hlt2GoodKaons" ]
Hlt2SharedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 

Hlt2SharedPhi2KK.DaughtersCuts = { "K+" : "ALL"} 
Hlt2SharedPhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<50*MeV)"
Hlt2SharedPhi2KK.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

Phi2KK = bindMembers( None, [ GoodKaons, Hlt2SharedPhi2KK ] )

### 
#
#  Hlt2 Phi for Bs -> Phi Phi selection
#
#  @author N.Styles n.a.styles@ed.ac.uk
#  @date 2008-10-09
#
##
Hlt2SharedUnbiasedPhi2KK = CombineParticles("Hlt2SharedUnbiasedPhi2KK")
Hlt2SharedUnbiasedPhi2KK.addTool(PhysDesktop)
Hlt2SharedUnbiasedPhi2KK.PhysDesktop.InputLocations = [ "Hlt2Kaons" ]
Hlt2SharedUnbiasedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 
Hlt2SharedUnbiasedPhi2KK.DaughtersCuts = { "K+" : "(PT>500)" } 
Hlt2SharedUnbiasedPhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<20*MeV)"
Hlt2SharedUnbiasedPhi2KK.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

UnbiasedPhi2KK = bindMembers( None, [ GoodKaons, Hlt2SharedUnbiasedPhi2KK ] )
