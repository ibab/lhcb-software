### @file
#
#  Some shared topological
#
#  @author 
#  @date 
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, FilterDesktop
from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
from Hlt2SharedParticles.TopoTF2Body import TopoTF2Body
from HltLine.HltLine import bindMembers

__all__ = ( 'TopoTF3Body' )

Hlt2SharedTopoTF3Body= CombineParticles("Hlt2SharedTopoTF3Body")

Hlt2SharedTopoTF3Body.InputLocations  = [ TopoTFInputParticles.outputSelection()
                                        , TopoTF2Body.outputSelection() ]
## Note doubly charged combinations allowed by Gabriel's original code.
Hlt2SharedTopoTF3Body.DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]

## Reminder: Default units of IP, DOCA, and VD are mm.
Hlt2SharedTopoTF3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
Hlt2SharedTopoTF3Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>100)"

TopoTF3Body = bindMembers( None, [ TopoTFInputParticles, TopoTF2Body, Hlt2SharedTopoTF3Body ] )

