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
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedElectrons
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'TrackFittedDiElectron'  )

Hlt2SharedTrackFittedDiElectron = Hlt2Member( CombineParticles
                                              , "TrackFittedDiElectron"
                                              , Inputs = [ BiKalmanFittedElectrons ]
                                              , DecayDescriptor = "J/psi(1S) -> e+ e-" 
                                              , DaughtersCuts = {"e+" : "(PT>1000*MeV)"}
                                              , CombinationCut = "AALL"
                                              , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                              )

TrackFittedDiElectron = bindMembers( "Shared", [ BiKalmanFittedElectrons, Hlt2SharedTrackFittedDiElectron ] )
