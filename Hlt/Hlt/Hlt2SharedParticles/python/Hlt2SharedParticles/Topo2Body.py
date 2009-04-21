### @file
#
#  2-body combinatorics for the topological trigger  
#
#  @author V. Gligorov,P. Spradlin 
#  @date 2009-04-21
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, PhysDesktop
from Hlt2SharedParticles.GoodParticles import GoodPions
from HltConf.HltLine import bindMembers

__all__ = ( 'Topo2Body2++','Topo2Body2--','Topo2Body2+-' )

Topo2Body2++ = CombineParticles("Topo2Body2++")
Topo2Body2++.addTool(PhysDesktop)
Topo2Body2++.PhysDesktop.InputLocations = [ "Hlt2GoodPions" ]
Topo2Body2++.DecayDescriptor = "K*(892)0 -> pi+ pi+" 

Topo2Body2++.DaughtersCuts = {"pi+" : "(ALL)"} 
Topo2Body2++.CombinationCut = "(AMAXCHILD(PT)>1.5*GeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
Topo2Body2++.MotherCut = "(BPVVD>2) & (BPVVDR > 0.2)"

Topo2Body2++ = bindMembers( None, [ GoodPions, Topo2Body2++ ] )

Topo2Body2+- = Topo2Body2++.clone("Topo2Body2+-")
Topo2Body2+-.DecayDescriptor = "K*(892)0 -> pi+ pi-"
Topo2Body2+- = bindMembers( None, [ GoodPions, Topo2Body2+- ] )

Topo2Body2-- = Topo2Body2++.clone("Topo2Body2--")
Topo2Body2--.DecayDescriptor = "K*(892)0 -> pi- pi-" 
Topo2Body2-- = bindMembers( None, [ GoodPions, Topo2Body2-- ] )

