### @file
#
#  Some shared topological
#
#  @author 
#  @date 
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop, FilterDesktop
from Hlt2SharedParticles.GoodParticles import GoodPions
from Hlt2SharedParticles.Topo2Body import *
from HltConf.HltLine import bindMembers

__all__ = ( 'Topo3Body2m2Body2pp', 'Topo3Body2p2Body2pm', 'Topo3Body2m2Body2pm', 'Topo3Body2p2Body2mm' )

Hlt2SharedTopo3Body= CombineParticles("Hlt2SharedTopo3Body")

Hlt2SharedTopo3Body.addTool(PhysDesktop)
Hlt2SharedTopo3Body.PhysDesktop.InputLocations  = ["Hlt2GoodPions", "Hlt2SharedTopo2Body"]
## Note doubly charged combinations allowed by Gabriel's original code.
Hlt2SharedTopo3Body.DecayDescriptors = ["D*(2010)+ -> pi+ K*(892)0", "D*(2010)+ -> pi- K*(892)0"]

## Reminder: Default units of IP, DOCA, and VD are mm.
Hlt2SharedTopo3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
Hlt2SharedTopo3Body.MotherCut = "(MAXTREE('pi+'==ABSID,PT)>1.5*GeV) & (BPVVD>2) & (BPVVDR > 0.2)"

Topo3Body = bindMembers( None, [ Topo2Body, Hlt2SharedTopo3Body ] )

