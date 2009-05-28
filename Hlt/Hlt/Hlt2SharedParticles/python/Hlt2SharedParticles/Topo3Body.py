### @file
#
#  3-body combinatorics for the topological trigger
#
#  @author V. Gligorov,P. Spradlin
#  @date 2009-04-21
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from Hlt2SharedParticles.GoodParticles import GoodPions
from Hlt2SharedParticles.Topo2Body import Topo2Body
from HltLine.HltLine import bindMembers

__all__ = ( 'Topo3Body' )

Hlt2SharedTopo3Body= CombineParticles("Hlt2SharedTopo3Body")

Hlt2SharedTopo3Body.InputLocations  = [GoodPions.outputSelection(), Topo2Body.outputSelection()]
## Note doubly charged combinations allowed by Gabriel's original code.
Hlt2SharedTopo3Body.DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]

## Reminder: Default units of IP, DOCA, and VD are mm.
Hlt2SharedTopo3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
Hlt2SharedTopo3Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVD>2) & (BPVVDR > 0.2)"

Topo3Body = bindMembers( None, [ GoodPions, Topo2Body, Hlt2SharedTopo3Body ] )

