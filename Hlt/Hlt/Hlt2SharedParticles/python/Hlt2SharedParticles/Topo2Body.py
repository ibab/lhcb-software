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
from Hlt2SharedParticles.GoodParticles import GoodPions
from HltConf.HltLine import bindMembers

__all__ = ( 'Topo2Body' )

Hlt2SharedTopo2Body = CombineParticles("Hlt2SharedTopo2Body")
Hlt2SharedTopo2Body.InputLocations = [ GoodPions.outputSelection() ]
Hlt2SharedTopo2Body.DecayDescriptors = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-"] 

Hlt2SharedTopo2Body.DaughtersCuts = {"pi+" : "(ALL)"} 
Hlt2SharedTopo2Body.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<0.1) & AALLSAMEBPV"
Hlt2SharedTopo2Body.MotherCut = "(MAXTREE('pi+'==ABSID,PT)>1.5*GeV) & (BPVVD>2) & (BPVVDR > 0.2)"

Topo2Body = bindMembers( None, [ GoodPions, Hlt2SharedTopo2Body ] )


