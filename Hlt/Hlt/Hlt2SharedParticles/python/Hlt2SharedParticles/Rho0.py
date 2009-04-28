### @file
#
#  Standard rho(770)0
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from HltConf.HltLine import bindMembers
from Hlt2SharedParticles.GoodParticles import GoodPions

__all__ = ('Rho0')

Hlt2SharedRho0 = CombineParticles("Hlt2SharedRho0")
Hlt2SharedRho0.InputLocations  = [ "Hlt2GoodPions" ]
Hlt2SharedRho0.DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc"
Hlt2SharedRho0.DaughtersCuts =  { "pi+" : "TRUE" ,
                                  "pi-" : "TRUE" }
Hlt2SharedRho0.CombinationCut = "(ADAMASS('rho(770)0')<100*MeV)"
Hlt2SharedRho0.MotherCut = "(BPVVDZ>0) & (VFASPF(VCHI2/VDOF)<9) & (BPVDIRA>0.95) & (BPVVDCHI2>25)"

Rho0 = bindMembers( None, [ GoodPions, Hlt2SharedRho0 ] )
