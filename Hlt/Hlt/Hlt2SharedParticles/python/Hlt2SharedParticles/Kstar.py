### @file
#
#  Standard K*
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
from Hlt2SharedParticles.TrackFittedBasicParticles import  BiKalmanFittedKaons, BiKalmanFittedPions
from Hlt2SharedParticles.TrackFittedBasicParticles import  BiKalmanFittedRichKaons
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'Kstar2KPi' , 'TightKstar2KPi' )

Hlt2SharedKstar2KPi = CombineParticles("Hlt2SharedKstar2KPi")
Hlt2SharedKstar2KPi.Inputs = [ BiKalmanFittedKaons.outputSelection(), BiKalmanFittedPions.outputSelection() ]
Hlt2SharedKstar2KPi.Output = 'Hlt2/Hlt2SharedKstar2KPi/Particles'
Hlt2SharedKstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
Hlt2SharedKstar2KPi.DaughtersCuts = { "pi+" : "ALL", "K+" : "ALL" } 
Hlt2SharedKstar2KPi.CombinationCut = "(ADAMASS('K*(892)0')<300)"
Hlt2SharedKstar2KPi.MotherCut = "(VFASPF(VCHI2PDOF)<25)"

Kstar2KPi = bindMembers( None, [ BiKalmanFittedKaons, BiKalmanFittedPions, Hlt2SharedKstar2KPi ] )


# Tight K* with PID

Hlt2SharedTightKstar2KPi = Hlt2Member( CombineParticles
                                       , "Hlt2SharedKstar2KPi"
                                       , Inputs = [ BiKalmanFittedRichKaons, BiKalmanFittedPions ]
                                       , DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
                                       , DaughtersCuts = { "pi+" : "(PT > 500*MeV)", "K+" : "(PT > 500*MeV) & (PIDK>0.)" }
                                       , CombinationCut = "(ADAMASS('K*(892)0')<100)"
                                       , MotherCut = "(VFASPF(VCHI2PDOF)<25)" )

TightKstar2KPi = bindMembers( 'Shared', [ BiKalmanFittedRichKaons, BiKalmanFittedPions, Hlt2SharedTightKstar2KPi ] )

