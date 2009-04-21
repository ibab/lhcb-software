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
from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
from HltConf.HltLine import bindMembers

__all__ = ( 'TopoTF2Body' )

Hlt2SharedTopoTF2Body = CombineParticles("Hlt2SharedTopoTF2Body")
Hlt2SharedTopoTF2Body.addTool(PhysDesktop)
Hlt2SharedTopoTF2Body.PhysDesktop.InputLocations = [ "Hlt2TopoTFInputParticles" ]
Hlt2SharedTopoTF2Body.DecayDescriptors = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-"] 

Hlt2SharedTopoTF2Body.DaughtersCuts = {"pi+" : "(ALL)"} 
Hlt2SharedTopoTF2Body.CombinationCut = "(AMAXCHILD(PT)>1.5*GeV) & (AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
Hlt2SharedTopoTF2Body.MotherCut = "(VFASPF(VCHI2/VDOF)<1000000) & (BPVVDCHI2>100)"

TopoTF2Body = bindMembers( None, [ TopoTFInputParticles, Hlt2SharedTopoTF2Body ] )


