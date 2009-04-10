###@file
#
# Standard rho+
# @ author Yasmine Amhis
# @ date 2008-07-17
##
#--------------------------------------------------------------------
# rho(770) -> pi pi0
#--------------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from HltConf.HltLine import bindMembers
from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons

__all__ = ( 'RhoPlus2PiPi0','RhoPlus2KPi0')

Hlt2SharedRhoPlus2PiPi0 = CombineParticles("Hlt2SharedRhoPlus2PiPi0")
# Special vertex fitter
Hlt2SharedRhoPlus2PiPi0.VertexFitters.update( { "" : "ParticleAdder"} ) 
Hlt2SharedRhoPlus2PiPi0.addTool(PhysDesktop)
Hlt2SharedRhoPlus2PiPi0.PhysDesktop.InputLocations = [ "Hlt2GoodPions"
                                                     , "Hlt2MergedPi0s"
                                                     , "Hlt2ResolvedPi0s"
                                                     ]
Hlt2SharedRhoPlus2PiPi0.DecayDescriptor = "[rho(770)- -> pi- pi0]cc"
Hlt2SharedRhoPlus2PiPi0.DaughtersCuts = { "pi+" : "ALL"
                                        , "pi0" :  "(PT>1000*MeV)  & (P> 1500*MeV)"}
#Note
#The cut of 550 MeV around the K*(892)+ is NOT a typo, is motivated
#by the requirements of the B2DVec HLT2 selection
Hlt2SharedRhoPlus2PiPi0.CombinationCut = "(ADAMASS('K*(892)+')<550*MeV)"
Hlt2SharedRhoPlus2PiPi0.MotherCut =  "ALL"

RhoPlus2PiPi0 = bindMembers( None, [ GoodPions, GaudiSequencer('SeqHlt2Neutrals'), Hlt2SharedRhoPlus2PiPi0 ])
#--------------------------------------------------------------------
#rho(770) -> K pi0 is a clone of rho(770) -> pi pi0
Hlt2SharedRhoPlus2KPi0 = Hlt2SharedRhoPlus2PiPi0.clone("Hlt2SharedRhoPlus2KPi0")
Hlt2SharedRhoPlus2KPi0.DecayDescriptor = "[rho(770)- -> K- pi0]cc"
Hlt2SharedRhoPlus2KPi0.PhysDesktop.InputLocations = [ "Hlt2GoodKaons"
                                                    , "Hlt2MergedPi0s"
                                                    , "Hlt2ResolvedPi0s"
                                                    ]
Hlt2SharedRhoPlus2KPi0.DaughtersCuts.update(  { "K+" : "ALL" } )
Hlt2SharedRhoPlus2KPi0.DaughtersCuts.pop( 'pi+' )


RhoPlus2KPi0 = bindMembers( None, [ GoodKaons, GaudiSequencer('SeqHlt2Neutrals'), Hlt2SharedRhoPlus2KPi0 ])


