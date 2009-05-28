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
from Hlt2SharedParticles.DXTF2BodyKs import DXTF2BodyKsDD
from HltLine.HltLine import bindMembers
from Hlt2SharedParticles.V0 import KsDD


__all__ = ( 'DXTF3BodyKsDD' )

Hlt2SharedDXTF3BodyKsDD= CombineParticles("Hlt2SharedDXTF3BodyKsDD")

Hlt2SharedDXTF3BodyKsDD.InputLocations  = [ TopoTFInputParticles.outputSelection()
                                          ,  DXTF2BodyKsDD.outputSelection() ]
## Note doubly charged combinations allowed by Gabriel's original code.
Hlt2SharedDXTF3BodyKsDD.DecayDescriptors = ["D*(2010)+ -> K*(892)0 pi+","D*(2010)+ -> K*(892)0 pi-","D*(2010)+ -> K*(892)0 K+","D*(2010)+ -> K*(892)0 K-"]

## Reminder: Default units of IP, DOCA, and VD are mm. 
Hlt2SharedDXTF3BodyKsDD.CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
Hlt2SharedDXTF3BodyKsDD.MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID) | ('KS0'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>100)"

DXTF3BodyKsDD = bindMembers( None, [ TopoTFInputParticles, DXTF2BodyKsDD, Hlt2SharedDXTF3BodyKsDD ] )


