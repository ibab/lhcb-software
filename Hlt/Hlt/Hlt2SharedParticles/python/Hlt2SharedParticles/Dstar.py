### @file
#
#  Standard D*
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
from Hlt2SharedParticles.D0 import D02KPi, D02KK, D02PiPi
from HltLine.HltLine import bindMembers

__all__ = ( 'DstarWithD02KPi', 'DstarWithD02KK', 'DstarWithD02PiPi' )

################################################################################
# Slow pion
Hlt2SlowPions = FilterDesktop("Hlt2SlowPions")
Hlt2SlowPions.InputLocations  = [ BiKalmanFittedPions.outputSelection(), "Hlt2VTTPions" ]
Hlt2SlowPions.Code = "(MIPCHI2DV(PRIMARY)>1)"

###@TODO:@FIXME add explicit Hlt2VTTPions dependency...
SlowPions = bindMembers( None, [ BiKalmanFittedPions, Hlt2SlowPions ] )
################################################################################
# D* with D0->Kpi
#
Hlt2SharedDstarWithD02KPi = CombineParticles("Hlt2SharedDstarWithD02KPi")
Hlt2SharedDstarWithD02KPi.InputLocations = [ SlowPions.outputSelection(),
                                             D02KPi.outputSelection() ]
Hlt2SharedDstarWithD02KPi.DecayDescriptors = ["[D*(2010)+ -> pi+ D0]cc",
                                              "[D*(2010)+ -> pi+ D~0]cc" ]

Hlt2SharedDstarWithD02KPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV) & (APT>1250*MeV)"
Hlt2SharedDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2PDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"

DstarWithD02KPi = bindMembers( None, [ SlowPions, D02KPi, Hlt2SharedDstarWithD02KPi ] )
################################################################################
# D* with D0->KK is a clone of D* with D0->Kpi
#
Hlt2SharedDstarWithD02KK = Hlt2SharedDstarWithD02KPi.clone("Hlt2SharedDstarWithD02KK")
Hlt2SharedDstarWithD02KK.DecayDescriptors = []
Hlt2SharedDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
Hlt2SharedDstarWithD02KK.InputLocations = [ SlowPions.outputSelection(),
                                            D02KK.outputSelection() ]

DstarWithD02KK = bindMembers( None, [ SlowPions, D02KK, Hlt2SharedDstarWithD02KK ] )
################################################################################
# D* with D0->PiPi is a clone of D* with D0->KK
#
Hlt2SharedDstarWithD02PiPi = Hlt2SharedDstarWithD02KK.clone("Hlt2SharedDstarWithD02PiPi")
Hlt2SharedDstarWithD02PiPi.InputLocations = [ SlowPions.outputSelection(),
                                              D02PiPi.outputSelection() ]
DstarWithD02PiPi = bindMembers( None, [ SlowPions, D02PiPi, Hlt2SharedDstarWithD02PiPi ] )
