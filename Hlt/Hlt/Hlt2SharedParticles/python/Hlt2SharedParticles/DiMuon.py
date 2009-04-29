### @file
#
#  Standard Di-muon
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.BasicParticles import Muons
from Configurables import CombineParticles, FilterDesktop
from HltConf.HltLine import bindMembers, Hlt2Member

__all__ = ( 'DiMuon' , 'Jpsi2MuMu' , 'HighPtJpsi2MuMu' )


Hlt2SharedDiMuon = Hlt2Member( CombineParticles
                             , "DiMuon"
                             , DecayDescriptor = "J/psi(1S) -> mu+ mu-"
                             , CombinationCut = "AALL"
                             , MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
                             , InputLocations = [ Muons ]
                             )

DiMuon = bindMembers( "Shared", [ Muons, Hlt2SharedDiMuon ] )
###
#
#  Standard J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedJpsi2MuMu = Hlt2Member( FilterDesktop
                                , "Jpsi2MuMu"
                                , Code = "ADMASS('J/psi(1S)')<100*MeV"
                                , InputLocations  = [ DiMuon ]
                                )

Jpsi2MuMu = bindMembers( "Shared", [ DiMuon, Hlt2SharedJpsi2MuMu ] )

###
#
#  High PT J/psi -> mumu
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-11-17
#
##
Hlt2SharedHighPtJpsi2MuMu = Hlt2Member( FilterDesktop
                                      , "HighPtJpsi2MuMu"
                                      , Code = "(PT>1*GeV) & (MINTREE(ABSID=='mu+',PT)>800*MeV) & (MAXTREE(ABSID=='mu+',TRCHI2DOF)<8) & (ADMASS('J/psi(1S)')<80*MeV)"
                                      , InputLocations  = [ Jpsi2MuMu ]
                                      )

HighPtJpsi2MuMu  = bindMembers( 'Shared', [ Jpsi2MuMu, Hlt2SharedHighPtJpsi2MuMu ] )
