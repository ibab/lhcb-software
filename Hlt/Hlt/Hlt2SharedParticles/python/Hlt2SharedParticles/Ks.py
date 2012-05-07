## from Sam ##### @file
## from Sam ###
## from Sam ### Standard Ks
## from Sam ###
## from Sam ###  @author P. Koppenburg Patrick.Koppenburg@cern.ch
## from Sam ###  @date 2008-07-15
## from Sam ###
## from Sam ####
## from Sam ##from Gaudi.Configuration import *
## from Sam ##from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions, BiKalmanFittedPions
## from Sam ##from Configurables import CombineParticles
## from Sam ##from HltLine.HltLine import bindMembers, Hlt2Member
## from Sam ##
## from Sam ##__all__ = ( 'KsLLTF', 'KsDD' )
## from Sam ##
## from Sam ##
## from Sam ### The LL K shorts, track fitted
## from Sam ##Hlt2SharedKsLLTF = Hlt2Member( CombineParticles, "KsLLTF"
## from Sam ##                           , DecayDescriptor = "KS0 -> pi+ pi-" 
## from Sam ##                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>36)" } 
## from Sam #### mds 120201                           , CombinationCut = "(ADAMASS('KS0')<50*MeV) & (ADOCACHI2CUT(30, ''))"
## from Sam ##                           , CombinationCut = "(ADAMASS('KS0')<50*MeV)"
## from Sam ##                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30) & (BPVLTIME() > 2.0*ps) "
## from Sam ##                           , Inputs = [ BiKalmanFittedPions ]
## from Sam ##                           )
## from Sam ##
## from Sam ##KsLLTF = bindMembers( "SharedKsLLTF", [BiKalmanFittedPions , Hlt2SharedKsLLTF ] )
## from Sam ##
## from Sam ### Now the downstream K shorts, requires fitted tracks!
## from Sam ##Hlt2SharedKsDD = Hlt2Member( CombineParticles, "KsDD"
## from Sam ##                           , DecayDescriptor = "KS0 -> pi+ pi-"
## from Sam ##                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT>300*MeV) & (MIPCHI2DV(PRIMARY)>9)" }
## from Sam #### mds 120201                           , CombinationCut = "(ADAMASS('KS0')<80*MeV) & (ADOCACHI2CUT(30, ''))"
## from Sam ##                           , CombinationCut = "(ADAMASS('KS0')<80*MeV)"
## from Sam ##                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2PDOF)<30)  & (BPVVDZ > 400)"
## from Sam ##                           , Inputs = [ BiKalmanFittedDownPions ]
## from Sam ##                           )
## from Sam ##
## from Sam ##KsDD = bindMembers( "SharedKsDD", [ BiKalmanFittedDownPions , Hlt2SharedKsDD ] )

## from http://lhcb-release-area.web.cern.ch/LHCb-release-area/HLT/HLT_v13r10p1/Hlt/Hlt2SharedParticles/python/Hlt2SharedParticles/Ks.py

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

__all__ = ( 'KsLL', 'KsDD' )

Hlt2SharedKsLL = Hlt2Member( CombineParticles, "KsLL"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" } 
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV)"
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30)"
                           , Inputs = [ GoodPions ]
                           )

KsLL = bindMembers( "SharedKsLL", [ GoodPions, Hlt2SharedKsLL ] )

# The LL K shorts, track fitted
Hlt2SharedKsLLTF = Hlt2Member( CombineParticles, "KsLLTF"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
## original code                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" } 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<3.)& (MIPCHI2DV(PRIMARY)>36)" } 
## original code                           , CombinationCut = "(ADAMASS('KS0')<50*MeV)"
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV) "
## original code                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30)"
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2PDOF)<30) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > 2.0*ps) "
                           , Inputs = [ BiKalmanFittedPions ]
                           )

KsLLTF = bindMembers( "SharedKsLLTF", [PV3D(),BiKalmanFittedPions , Hlt2SharedKsLLTF ] )

# Now the downstream K shorts, requires fitted tracks!
Hlt2SharedKsDD = Hlt2Member( CombineParticles, "KsDD"
                           , DecayDescriptor = "KS0 -> pi+ pi-"
## original code                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" }
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4) & (P>3000*MeV) & (PT > 175.*MeV)" }
                           , CombinationCut = "(ADAMASS('KS0')<80*MeV)"
## original code                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2PDOF)<30)"
                           , MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2PDOF)<30)  & (BPVVDZ > 400.0*mm)"
                           , Inputs = [ BiKalmanFittedDownPions ]
                           )

KsDD = bindMembers( "SharedKsDD", [ PV3D(),BiKalmanFittedDownPions , Hlt2SharedKsDD ] )

