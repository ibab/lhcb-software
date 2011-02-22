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

__all__ = ( 'TrackFittedDiMuon' , 'TrackFittedJpsi2MuMu' , 'TrackFittedHighPtJpsi2MuMu' )


Hlt2SharedTrackFittedDiMuon = Hlt2Member( CombineParticles
                                          , "TrackFittedDiMuon"
                                          , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
                                          , CombinationCut = "AALL"
                                          , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                                          , Inputs = [ BiKalmanFittedMuons ]
                                          , WriteP2PVRelations = False
                                          )

TrackFittedDiMuon = bindMembers( "Shared", [ BiKalmanFittedMuons, Hlt2SharedTrackFittedDiMuon ] )

###
#
#  Standard Track Fitted J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedTrackFittedJpsi2MuMu = Hlt2Member( FilterDesktop
                                             , "TrackFittedJpsi2MuMu"
                                             , Code = "ADMASS('J/psi(1S)')<120*MeV"
                                             , Inputs  = [ TrackFittedDiMuon ]
                                             )

TrackFittedJpsi2MuMu = bindMembers( "Shared", [ TrackFittedDiMuon, Hlt2SharedTrackFittedJpsi2MuMu ] )

###
#
#  Detached Track Fitted J/psi -> mumu
#
#  @author R. Aaij roel.aaij@cern.ch
#  @date 2010-8-23
#
##
Hlt2SharedDetachedTrackFittedJpsi2MuMu = Hlt2Member( FilterDesktop
                                                     , "DetachedTrackFittedJpsi2MuMu"
                                                     , Code = "BPVDLS > 3"
                                                     , Inputs  = [ TrackFittedJpsi2MuMu ]
                                                     )

DetachedTrackFittedJpsi2MuMu = bindMembers( "Shared", [ TrackFittedJpsi2MuMu,
                                                        Hlt2SharedDetachedTrackFittedJpsi2MuMu ] )

###
#
#  Track Fitted High PT J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedTrackFittedHighPtJpsi2MuMu = Hlt2Member( FilterDesktop
                                                   , "TrackFittedHighPtJpsi2MuMu"
                                                   , Code = "(PT>1*GeV) & (MINTREE(ABSID=='mu+',PT)>800*MeV) & (MAXTREE(ABSID=='mu+',TRCHI2DOF)<8) & (ADMASS('J/psi(1S)')<80*MeV)"
                                                   , Inputs  = [ TrackFittedJpsi2MuMu ]
                                      )

TrackFittedHighPtJpsi2MuMu  = bindMembers( 'Shared', [ TrackFittedJpsi2MuMu, Hlt2SharedTrackFittedHighPtJpsi2MuMu ] )
