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

__all__ = ( 'LambdaLLTrackFitted', 'LambdaDDTrackFitted' )

##################################################
# Standard Lambda: Fitted Long Tracks
Hlt2SharedLambdaLLTrackFitted = Hlt2Member( CombineParticles, "LambdaLLTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
                                                                "p+"  : "(TRCHI2DOF<20)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<50*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<100*MeV) & (VFASPF(VCHI2PDOF)<50)"
                                            , InputLocations = [ BiKalmanFittedPions, BiKalmanFittedProtons ]
                                            )

##################################################
# Standard Lambda: Fitted Downstream Tracks
Hlt2SharedLambdaDDTrackFitted = Hlt2Member( CombineParticles, "LambdaDDTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
                                                                "p+"  : "(TRCHI2DOF<20)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<100*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<100*MeV) & (VFASPF(VCHI2PDOF)<50)"
                                            , InputLocations = [ BiKalmanFittedDownPions, BiKalmanFittedDownProtons ]
                                            )

LambdaLLTrackFitted = bindMembers( "Shared", [ BiKalmanFittedPions, BiKalmanFittedProtons, Hlt2SharedLambdaLLTrackFitted ] )
LambdaDDTrackFitted = bindMembers( "Shared", [ BiKalmanFittedDownPions, BiKalmanFittedDownProtons, Hlt2SharedLambdaDDTrackFitted ] )


