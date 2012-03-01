### @file
#
#  Standard Track Fitted Di-electron
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons, BiKalmanFittedElectronsFromL0
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'TrackFittedDiElectron' , 'TrackFittedDiElectronFromL0'  )

Hlt2SharedTrackFittedDiElectron = Hlt2Member( CombineParticles
                                              , "TrackFittedDiElectron"
                                              , Inputs = [ BiKalmanFittedElectrons ]
                                              , DecayDescriptor = "J/psi(1S) -> e+ e-" 
                                              , DaughtersCuts = {"e+" : "(PT>1000*MeV)"}
                                              , CombinationCut = "AALL"
                                              , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                              )

Hlt2SharedTrackFittedDiElectronFromL0 = Hlt2Member( CombineParticles
                                                    , "TrackFittedDiElectronFromL0"
                                                    , Inputs = [ BiKalmanFittedElectronsFromL0 ]
                                                    , DecayDescriptor = "J/psi(1S) -> e+ e-" 
                                                    , DaughtersCuts = {"e+" : "(PT>1000*MeV)"}
                                                    , CombinationCut = "AALL"
                                                    , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                                    )

TrackFittedDiElectron = bindMembers( "Shared", [ BiKalmanFittedElectrons, Hlt2SharedTrackFittedDiElectron ] )
TrackFittedDiElectronFromL0 = bindMembers( "SharedFromL0", [ BiKalmanFittedElectronsFromL0, Hlt2SharedTrackFittedDiElectronFromL0 ] )
