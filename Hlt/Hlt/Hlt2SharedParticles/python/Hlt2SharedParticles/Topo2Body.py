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

__all__ = ( 'Topo2Body2pp','Topo2Body2mm','Topo2Body2pm' )

Hlt2SharedTopo2Body2pp = CombineParticles("Hlt2SharedTopo2Body2pp")
Hlt2SharedTopo2Body2pp.addTool(PhysDesktop)
Hlt2SharedTopo2Body2pp.PhysDesktop.InputLocations = [ "Hlt2GoodPions" ]
Hlt2SharedTopo2Body2pp.DecayDescriptor = "K*(892)0 -> pi+ pi+" 

Hlt2SharedTopo2Body2pp.DaughtersCuts = {"pi+" : "(ALL)"} 
Hlt2SharedTopo2Body2pp.CombinationCut = "(AMAXCHILD(PT)>1.5*GeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
Hlt2SharedTopo2Body2pp.MotherCut = "(BPVVD>2) & (BPVVDR > 0.2)"

Topo2Body2pp = bindMembers( None, [ GoodPions, Hlt2SharedTopo2Body2pp ] )

Hlt2SharedTopo2Body2pm = Hlt2SharedTopo2Body2pp.clone("Hlt2SharedTopo2Body2pm")
Hlt2SharedTopo2Body2pm.DecayDescriptor = "K*(892)0 -> pi+ pi-"
Topo2Body2pm = bindMembers( None, [ GoodPions, Hlt2SharedTopo2Body2pm ] )

Hlt2SharedTopo2Body2mm = Topo2Body2pp.clone("Hlt2SharedTopo2Body2mm")
Hlt2SharedTopo2Body2mm.DecayDescriptor = "K*(892)0 -> pi- pi-" 
Topo2Body2mm = bindMembers( None, [ GoodPions, Hlt2SharedTopo2Body2mm ] )

