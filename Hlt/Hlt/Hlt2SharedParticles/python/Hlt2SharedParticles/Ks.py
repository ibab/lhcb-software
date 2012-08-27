## @file
#
# Standard Ks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.GoodParticles import GoodPions
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions, BiKalmanFittedPions
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D

__all__ = ( 'KsLLTF', 'KsDD' )

# The LL K shorts, track fitted
Hlt2SharedKsLLTF = Hlt2Member( CombineParticles, "KsLLTF"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<3.)& (MIPCHI2DV(PRIMARY)>36)" } 
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV) "
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > 2.0*ps) "
                           , Inputs = [ BiKalmanFittedPions ]
                           )

KsLLTF = bindMembers( "SharedKsLLTF", [PV3D(),BiKalmanFittedPions , Hlt2SharedKsLLTF ] )

# Now the downstream K shorts, requires fitted tracks!
Hlt2SharedKsDD = Hlt2Member( CombineParticles, "KsDD"
                           , DecayDescriptor = "KS0 -> pi+ pi-"
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT > 175.*MeV)" }
                           , CombinationCut = "(ADAMASS('KS0')<80*MeV)"
                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2PDOF)<30)  & (BPVVDZ > 400.0*mm)"
                           , Inputs = [ BiKalmanFittedDownPions ]
                           )

KsDD = bindMembers( "SharedKsDD", [ PV3D(),BiKalmanFittedDownPions , Hlt2SharedKsDD ] )

