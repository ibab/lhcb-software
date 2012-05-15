### @file
#
# Standard Lambda
#  @author F. Dettori Francesco.Dettori@cern.ch
#  Based on code by P. Koppenburg 
#  @date 2010-03-18
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedDownPions, BiKalmanFittedProtons, BiKalmanFittedDownProtons
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers, Hlt2Member
from HltTracking.HltPVs import PV3D
__all__ = ( 'LambdaLLTrackFitted', 'LambdaDDTrackFitted' )

##################################################
# Standard Lambda: Fitted Long Tracks
Hlt2SharedLambdaLLTrackFitted = Hlt2Member( CombineParticles, "LambdaLLTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4)& (MIPCHI2DV(PRIMARY)>36)",
                                                                "p+"  : "(TRCHI2DOF<4)& (MIPCHI2DV(PRIMARY)>36)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<50*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<20*MeV) & (VFASPF(VCHI2PDOF)<30) & (BPVLTIME('PropertimeFitter/properTime:PUBLIC') > 2.0*ps) "
                                            , Inputs = [ BiKalmanFittedPions, BiKalmanFittedProtons ]
                                            )

##################################################
# Standard Lambda: Fitted Downstream Tracks
Hlt2SharedLambdaDDTrackFitted = Hlt2Member( CombineParticles, "LambdaDDTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<4)& (P>3000*MeV)  & (PT>175.*MeV)",
                                                                "p+"  : "(TRCHI2DOF<4)& (P>3000*MeV)  & (PT>175.*MeV)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<80*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<64*MeV)& (VFASPF(VCHI2PDOF)<30)& (BPVVDZ > 400.0*mm)"
                                            , Inputs = [ BiKalmanFittedDownPions, BiKalmanFittedDownProtons ]
                                            )

LambdaLLTrackFitted = bindMembers( "Shared", [ PV3D(), BiKalmanFittedPions, BiKalmanFittedProtons, Hlt2SharedLambdaLLTrackFitted ] )
LambdaDDTrackFitted = bindMembers( "Shared", [ PV3D(), BiKalmanFittedDownPions, BiKalmanFittedDownProtons, Hlt2SharedLambdaDDTrackFitted ] )


