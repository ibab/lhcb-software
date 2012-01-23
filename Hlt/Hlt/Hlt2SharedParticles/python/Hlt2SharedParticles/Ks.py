### @file
#
# Standard Ks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions, BiKalmanFittedPions
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'KsLLTF', 'KsDD' )


# The LL K shorts, track fitted
Hlt2SharedKsLLTF = Hlt2Member( CombineParticles, "KsLLTF"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>36)" } 
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV) & (ADOCACHI2CUT(30, ''))"
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30) & (BPVLTIME() > 2.0*ps) "
                           , Inputs = [ BiKalmanFittedPions ]
                           )

KsLLTF = bindMembers( "SharedKsLLTF", [BiKalmanFittedPions , Hlt2SharedKsLLTF ] )

# Now the downstream K shorts, requires fitted tracks!
Hlt2SharedKsDD = Hlt2Member( CombineParticles, "KsDD"
                           , DecayDescriptor = "KS0 -> pi+ pi-"
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>9)" }
                           , CombinationCut = "(ADAMASS('KS0')<80*MeV) & (ADOCACHI2CUT(30, ''))"
                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2PDOF)<30)  & (BPVVDZ > 400)"
                           , Inputs = [ BiKalmanFittedDownPions ]
                           )

KsDD = bindMembers( "SharedKsDD", [ BiKalmanFittedDownPions , Hlt2SharedKsDD ] )
