### @file
#
#  2-body combinatorics for the topological trigger  
#
#  @author V. Gligorov,P. Spradlin 
#  @date 2009-04-21
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
from HltConf.HltLine import bindMembers

__all__ = ( 'TopoTF2Body' )

Hlt2SharedTopoTF2Body = CombineParticles("Hlt2SharedTopoTF2Body")
Hlt2SharedTopoTF2Body.InputLocations = [ TopoTFInputParticles.outputSelection() ]
Hlt2SharedTopoTF2Body.DecayDescriptors = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]

Hlt2SharedTopoTF2Body.DaughtersCuts = {"pi+" : "(ALL)", "K+" : "(ALL)"}

Hlt2SharedTopoTF2Body.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
Hlt2SharedTopoTF2Body.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>100)"

TopoTF2Body = bindMembers( None, [ TopoTFInputParticles, Hlt2SharedTopoTF2Body ] )


