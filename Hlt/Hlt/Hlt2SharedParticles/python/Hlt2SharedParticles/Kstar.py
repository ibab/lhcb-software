### @file
#
#  Standard K*
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from Hlt2SharedParticles.GoodParticles import GoodKaons, GoodPions
from HltConf.HltLine import bindMembers

__all__ = ( 'Kstar2KPi' )

Hlt2SharedKstar2KPi = CombineParticles("Hlt2SharedKstar2KPi")
Hlt2SharedKstar2KPi.addTool(PhysDesktop)
Hlt2SharedKstar2KPi.PhysDesktop.InputLocations = [ "Hlt2GoodKaons", "Hlt2GoodPions" ]
Hlt2SharedKstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 

Hlt2SharedKstar2KPi.DaughtersCuts = { "pi+" : "ALL", "K+" : "ALL" } 
Hlt2SharedKstar2KPi.CombinationCut = "(ADAMASS('K*(892)0')<300)"
Hlt2SharedKstar2KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

Kstar2KPi = bindMembers( None, [ GoodKaons, GoodPions, Hlt2SharedKstar2KPi ] )
