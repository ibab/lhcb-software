### @file
#
#  Standard Track Fitted Di-muon
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons
from Configurables import CombineParticles, FilterDesktop
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'TrackFittedDiMuon' )


Hlt2SharedTrackFittedDiMuon = Hlt2Member( CombineParticles
                                          , "TrackFittedDiMuon"
                                          , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
                                          , CombinationCut = "AALL"
                                          , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                          , Inputs = [ BiKalmanFittedMuons ]
                                          , WriteP2PVRelations = False
                                          )

TrackFittedDiMuon = bindMembers( "Shared", [ BiKalmanFittedMuons, Hlt2SharedTrackFittedDiMuon ] )
