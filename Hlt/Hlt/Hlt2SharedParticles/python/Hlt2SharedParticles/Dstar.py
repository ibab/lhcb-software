### @file
#
#  Standard D*
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop, FilterDesktop
from Hlt2SharedParticles.BasicParticles import NoCutsPions
from Hlt2SharedParticles.D0 import D02KPi, D02KK, D02PiPi
from HltConf.HltLine import bindMembers

__all__ = ( 'DstarWithD02KPi', 'DstarWithD02KK', 'DstarWithD02PiPi' )

################################################################################
# Slow pion
Hlt2SlowPions = FilterDesktop("Hlt2SlowPions")
Hlt2SlowPions.addTool(PhysDesktop)
Hlt2SlowPions.PhysDesktop.InputLocations  = [ "Hlt2NoCutsPions", "Hlt2VTTPions" ]
Hlt2SlowPions.Code = "(MIPCHI2DV(PRIMARY)>1)"

###@TODO:@FIXME add explicit Hlt2Pions, Hlt2VTTPions dependency...
SlowPions = bindMembers( None, [ NoCutsPions, Hlt2SlowPions ] )
################################################################################
# D* with D0->Kpi
#
Hlt2SharedDstarWithD02KPi = CombineParticles("Hlt2SharedDstarWithD02KPi")
Hlt2SharedDstarWithD02KPi.addTool(PhysDesktop)
Hlt2SharedDstarWithD02KPi.PhysDesktop.InputLocations = [ "Hlt2SlowPions",
                                                         "Hlt2SharedD02KPi" ]
Hlt2SharedDstarWithD02KPi.DecayDescriptors = ["[D*(2010)+ -> pi+ D0]cc",
                                              "[D*(2010)+ -> pi+ D~0]cc" ]

Hlt2SharedDstarWithD02KPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV) & (APT>1250*MeV)"
Hlt2SharedDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"

DstarWithD02KPi = bindMembers( None, [ SlowPions, D02KPi, Hlt2SharedDstarWithD02KPi ] )
################################################################################
# D* with D0->KK is a clone of D* with D0->Kpi
#
Hlt2SharedDstarWithD02KK = Hlt2SharedDstarWithD02KPi.clone("Hlt2SharedDstarWithD02KK")
Hlt2SharedDstarWithD02KK.DecayDescriptors = []
Hlt2SharedDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
Hlt2SharedDstarWithD02KK.addTool(PhysDesktop)
Hlt2SharedDstarWithD02KK.PhysDesktop.InputLocations = [ "Hlt2SlowPions",
                                                         "Hlt2SharedD02KK" ]

DstarWithD02KK = bindMembers( None, [ SlowPions, D02KK, Hlt2SharedDstarWithD02KK ] )
################################################################################
# D* with D0->PiPi is a clone of D* with D0->KK
#
Hlt2SharedDstarWithD02PiPi = Hlt2SharedDstarWithD02KK.clone("Hlt2SharedDstarWithD02PiPi")
Hlt2SharedDstarWithD02PiPi.addTool(PhysDesktop)
Hlt2SharedDstarWithD02PiPi.PhysDesktop.InputLocations = [ "Hlt2SlowPions",
                                                         "Hlt2SharedD02PiPi" ]
DstarWithD02PiPi = bindMembers( None, [ SlowPions, D02PiPi, Hlt2SharedDstarWithD02PiPi ] )
