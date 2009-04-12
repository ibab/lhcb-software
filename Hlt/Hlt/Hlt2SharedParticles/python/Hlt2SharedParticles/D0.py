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
from Configurables import CombineParticles, GaudiSequencer, PhysDesktop
from HltConf.HltLine import bindMembers

__all__ = ( 'D02KPi', 'D02PiPi', 'D02KK', 'D02KsPiPi', 'D02KsKK' )
##########################################################################################
# D0 -> K Pi
#
Hlt2SharedD02KPi = CombineParticles("Hlt2SharedD02KPi")
Hlt2SharedD02KPi.DecayDescriptor = "[D0 -> K- pi+]cc" 
Hlt2SharedD02KPi.DaughtersCuts = { "K+" : "ALL",
                                  "pi+" : "ALL"
Hlt2SharedD02KPi.CombinationCut = "(ADAMASS('D0')<50*MeV) & (APT>1*GeV)"
Hlt2SharedD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
Hlt2SharedD02KPi.addTool(PhysDesktop)
Hlt2SharedD02KPi.PhysDesktop.InputLocations = [ "Hlt2GoodKaons", "Hlt2GoodPions" ]

D02KPi = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedD02KPi ] )
##########################################################################################
# D0 -> Pi Pi is a clone of K Pi
#
Hlt2SharedD02PiPi = Hlt2SharedD02KPi.clone("Hlt2SharedD02PiPi")
Hlt2SharedD02PiPi.DecayDescriptor = "[D0 -> pi- pi+]cc"
Hlt2SharedD02PiPi.PhysDesktop.InputLocations = [ "Hlt2GoodPions" ]

D02PiPi = bindMembers( None, [ GoodPions, Hlt2SharedD02PiPi ] )
##########################################################################################
# D0 -> K K
#
Hlt2SharedD02KK = Hlt2SharedD02KPi.clone("Hlt2SharedD02KK")
Hlt2SharedD02KK.DecayDescriptor = "[D0 -> K- K+]cc" 
Hlt2SharedD02KK.PhysDesktop.InputLocations = [ "Hlt2GoodKaons" ]

D02KK = bindMembers( None, [ GoodKaons, Hlt2SharedD02KK ] )
##########################################################################################
# D0 -> Ks Pi Pi
#
Hlt2SharedD02KsPiPi = CombineParticles("Hlt2SharedD02KsPiPi")
Hlt2SharedD02KsPiPi.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc" 
Hlt2SharedD02KsPiPi.DaughtersCuts = { "pi+" : "(PT>400*MeV)", "K+" : "(PT>400*MeV)",
                                      "KS0" : "(PT>1*GeV)"} 
Hlt2SharedD02KsPiPi.CombinationCut = "(ADAMASS('D0')<80*MeV) & (APT>1800*MeV)"
Hlt2SharedD02KsPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>1)"
Hlt2SharedD02KsPiPi.addTool(PhysDesktop)

Hlt2SharedD02KsPiPi.PhysDesktop.InputLocations = [ "Hlt2GoodPions", "Hlt2KsLLParticles", "Hlt2SharedKsLL" ]

###@TODO:@FIXME: convert Hl2Particles.opts to python... 
importOptions('$HLTCONFROOT/options/Hlt2Particles.opts')
# Need to do an 'OR' of SeqMakeHlt2KsLL and KsLL...
_KsLL =  GaudiSequencer('SeqMakeHltSharedKs_', ModeOR=True, ShortCircuit=False, Members = [ GaudiSequencer('SeqMakeHlt2KsLL') ] + KsLL.members())
####@TODO:@FIXME: maybe we need seperate D0 -> Ks_1 Pi Pi, D0 -> Ks_2 Pi Pi instead of the this OR...


D02KsPiPi = bindMembers( None, [ GoodPions, _KsLL , Hlt2SharedD02KsPiPi ] )
##########################################################################################
# D0 -> Ks K K is a clone of Ks Pi Pi
#
Hlt2SharedD02KsKK = Hlt2SharedD02KsPiPi.clone("Hlt2SharedD02KsKK")
Hlt2SharedD02KsKK.DecayDescriptor = "[D0 -> KS0 K+ K-]cc" 
Hlt2SharedD02KsKK.DaughtersCuts = { "K+" : "(PT>300*MeV)",
                                   "KS0" : "(PT>800*MeV)" } 
Hlt2SharedD02KsKK.CombinationCut = "(ADAMASS('D0')<80) & (APT>1500*MeV)" 
Hlt2SharedD02KsKK.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>0.49)" 
Hlt2SharedD02KsKK.addTool(PhysDesktop)
Hlt2SharedD02KsKK.PhysDesktop.InputLocations = [ "Hlt2GoodKaons", "Hlt2KsLLParticles", "Hlt2SharedKsLL" ]


D02KsKK = bindMembers( None, [ GoodKaons, _KsLL , Hlt2SharedD02KsKK ] )
