### @file
#
#  Standard D0
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
from Hlt2SharedParticles.Ks import KsLL
from Hlt2SharedParticles.V0 import KsLL as KsV0LL
from Configurables import CombineParticles, GaudiSequencer
from HltLine.HltLine import Hlt2Member
from HltLine.HltLine import bindMembers

__all__ = ( 'D02KPi', 'D02PiPi', 'D02KK', 'D02KsPiPi', 'D02KsKK' )
##########################################################################################
# D0 -> K Pi
#
Hlt2SharedD02KPi = Hlt2Member( CombineParticles
                             , "D02KPi"
                             , DecayDescriptor = "[D0 -> K- pi+]cc" 
                             , DaughtersCuts = {  "K+" : "ALL"
                                               , "pi+" : "ALL" }
                             , CombinationCut = "(ADAMASS('D0')<50*MeV) & (APT>1*GeV)"
                             , MotherCut = "(VFASPF(VCHI2PDOF)<25)"
                             , Inputs = [ GoodKaons, GoodPions ]
                             )
D02KPi = bindMembers( 'Shared', [ GoodKaons, GoodPions, Hlt2SharedD02KPi ] )
##########################################################################################
# D0 -> Pi Pi is a clone of K Pi
#
Hlt2SharedD02PiPi = Hlt2SharedD02KPi.clone("D02PiPi"
                                          , DecayDescriptor = "[D0 -> pi- pi+]cc"
                                          , Inputs = [ GoodPions ]
                                          )
D02PiPi = bindMembers( 'Shared', [ GoodPions, Hlt2SharedD02PiPi ] )
##########################################################################################
# D0 -> K K
#
Hlt2SharedD02KK = Hlt2SharedD02KPi.clone("D02KK"
                                        , DecayDescriptor = "[D0 -> K- K+]cc" 
                                        , Inputs = [ GoodKaons ]
                                        )
D02KK = bindMembers( 'Shared', [ GoodKaons, Hlt2SharedD02KK ] )
##########################################################################################
# D0 -> Ks Pi Pi
#
Hlt2SharedD02KsPiPi = Hlt2Member( CombineParticles
                                , "D02KsPiPi"
                                , DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc" 
                                , DaughtersCuts = { "pi+" : "(PT>400*MeV)"
                                                  , "K+"  : "(PT>400*MeV)"
                                                  , "KS0" : "(PT>1*GeV)"} 
                                , CombinationCut = "(ADAMASS('D0')<80*MeV) & (APT>1800*MeV)"
                                , MotherCut = "(VFASPF(VCHI2PDOF)<10) & (MIPCHI2DV(PRIMARY)>1)"
                                , Inputs = [ GoodPions, KsV0LL, KsLL ]
                                )

_KsLL =  bindMembers( None
                    , [ GaudiSequencer('SeqKsForHlt2SharedD02KsHH', ModeOR=True, ShortCircuit=False, Members = KsV0LL.members()  + KsLL.members()) ]
                    ).setOutputSelection( [ KsV0LL.outputSelection(), KsLL.outputSelection()] )
####@TODO:@FIXME: maybe we need seperate D0 -> Ks_1 Pi Pi, D0 -> Ks_2 Pi Pi instead of the this OR...


D02KsPiPi = bindMembers( "Shared", [ GoodPions, _KsLL , Hlt2SharedD02KsPiPi ] )
##########################################################################################
# D0 -> Ks K K is a clone of Ks Pi Pi
#
Hlt2SharedD02KsKK = Hlt2SharedD02KsPiPi.clone("D02KsKK"
                                             , DecayDescriptor = "[D0 -> KS0 K+ K-]cc" 
                                             , DaughtersCuts = { "K+" : "(PT>300*MeV)",
                                                                 "KS0" : "(PT>800*MeV)" } 
                                             , CombinationCut = "(ADAMASS('D0')<80) & (APT>1500*MeV)" 
                                             , MotherCut = "(VFASPF(VCHI2PDOF)<10) & (MIPCHI2DV(PRIMARY)>0.49)" 
                                             , Inputs = [ GoodKaons, KsV0LL, KsLL ]
                                             )

D02KsKK = bindMembers( "Shared", [ GoodKaons, _KsLL , Hlt2SharedD02KsKK ] )
