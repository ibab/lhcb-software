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
from Hlt2SharedParticles.V0 import KsDDFit

__all__ = ( 'DXTF2BodyKsDD' )

Hlt2SharedDXTF2BodyDD = CombineParticles("Hlt2SharedDXTF2BodyDD")
Hlt2SharedDXTF2BodyDD.InputLocations = [ TopoTFInputParticles.outputSelection(), KsDDFit.outputSelection() ]
Hlt2SharedDXTF2BodyDD.DecayDescriptors = ["K*(892)0 -> KS0 pi+","K*(892)0 -> KS0 pi-","K*(892)0 -> KS0 K+","K*(892)0 -> KS0 K-"]

Hlt2SharedDXTF2BodyDD.DaughtersCuts = {"pi+" : "(ALL)", "K+" : "(ALL)", "KS0" : "(ALL)"}

#Hlt2SharedDXTF2BodyDD.CombinationCut = "(AMINDOCA('LoKi::TrgDistanceCalculator')<1) & AALLSAMEBPV"
Hlt2SharedDXTF2BodyDD.CombinationCut = "AALLSAMEBPV"
Hlt2SharedDXTF2BodyDD.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID) | ('KS0'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>100)"

DXTF2BodyKsDD = bindMembers( None, [ TopoTFInputParticles, KsDDFit, Hlt2SharedDXTF2BodyDD ] )


