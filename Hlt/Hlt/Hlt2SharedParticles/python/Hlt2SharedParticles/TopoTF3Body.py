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
from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
from Hlt2SharedParticles.TopoTF2Body import TopoTF2Body
from HltConf.HltLine import bindMembers

__all__ = ( 'TopoTF3Body' )

Hlt2SharedTopoTF3Body= CombineParticles("Hlt2SharedTopoTF3Body")

Hlt2SharedTopoTF3Body.addTool(PhysDesktop)
Hlt2SharedTopoTF3Body.PhysDesktop.InputLocations  = ["Hlt2TopoTFInputParticles", "Hlt2SharedTopoTF2Body"]
## Note doubly charged combinations allowed by Gabriel's original code.
Hlt2SharedTopoTF3Body.DecayDescriptors = ["D*(2010)+ -> pi+ K*(892)0", "D*(2010)+ -> pi- K*(892)0"]

## Reminder: Default units of IP, DOCA, and VD are mm.
Hlt2SharedTopoTF3Body.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
Hlt2SharedTopoTF3Body.MotherCut = "(MAXTREE('pi+'==ABSID,PT)>1.5*GeV) & (BPVVDCHI2>100)"

TopoTF3Body = bindMembers( None, [ TopoTFInputParticles, TopoTF2Body, Hlt2SharedTopoTF3Body ] )

