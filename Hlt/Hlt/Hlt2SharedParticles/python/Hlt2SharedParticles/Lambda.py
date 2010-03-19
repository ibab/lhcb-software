### @file
#
# Standard Lambda
#  @author F. Dettori Francesco.Dettori@cern.ch
#  Based on code by P. Koppenburg 
#  @date 2010-03-18
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.BasicParticles import NoCutsPions, NoCutsProtons
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedDownPions, BiKalmanFittedProtons, BiKalmanFittedDownProtons
from Configurables import CombineParticles
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'LambdaLL','LambdaLLTrackFitted', 'LambdaDDTrackFitted' )

##################################################
# Standard Lambda: Long Tracks
Hlt2SharedLambdaLL = Hlt2Member( CombineParticles, "LambdaLL"
                                 , DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 
                                 , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
                                                     "p+"  : "(TRCHI2DOF<20)"} 
                                 , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                 , MotherCut = "(ADMASS('Lambda0')<15*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                                 , InputLocations = [ NoCutsPions, NoCutsProtons ]
                                 )
##################################################
# Standard Lambda: Fitted Long Tracks
Hlt2SharedLambdaLLTrackFitted = Hlt2Member( CombineParticles, "LambdaLLTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
                                                                "p+"  : "(TRCHI2DOF<20)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<15*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                                            , InputLocations = [ BiKalmanFittedPions, BiKalmanFittedProtons ]
                                            )

##################################################
# Standard Lambda: Downstream Tracks
# are there unfitted downstream tracks??
## Hlt2SharedLambdaDD = Hlt2Member( CombineParticles, "LambdaDD"
##                                  , DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 
##                                  , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
##                                                      "p+"  : "(TRCHI2DOF<20)"} 
##                                  , CombinationCut = "(ADAMASS('Lambda0')<100*MeV)"
##                                  , MotherCut = "(ADMASS('Lambda0')<50*MeV) & (VFASPF(VCHI2/VDOF)<30)"
##                                  , InputLocations = [ NoCutsPions, NoCutsProtons ]
##                                  )

##################################################
# Standard Lambda: Fitted Downstream Tracks
Hlt2SharedLambdaDDTrackFitted = Hlt2Member( CombineParticles, "LambdaDDTrackFitted"
                                            , DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
                                            , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)",
                                                                "p+"  : "(TRCHI2DOF<20)"} 
                                            , CombinationCut = "(ADAMASS('Lambda0')<30*MeV)"
                                            , MotherCut = "(ADMASS('Lambda0')<15*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                                            , InputLocations = [ BiKalmanFittedDownPions, BiKalmanFittedDownProtons ]
                                            )

LambdaLL            = bindMembers( "SharedLambdaLL",            [ NoCutsPions, NoCutsProtons, Hlt2SharedLambdaLL ] )
LambdaLLTrackFitted = bindMembers( "SharedLambdaLLTrackFitted", [ BiKalmanFittedPions, BiKalmanFittedProtons, Hlt2SharedLambdaLL ] )
#LambdaDD            = bindMembers( "SharedLambdaDD",            [ NoCutsPions, NoCutsProtons, Hlt2SharedLambdaDD ] )
LambdaDDTrackFitted = bindMembers( "SharedLambdaDDTrackFitted", [ BiKalmanFittedDownPions, BiKalmanFittedDownProtons, Hlt2SharedLambdaDD ] )


