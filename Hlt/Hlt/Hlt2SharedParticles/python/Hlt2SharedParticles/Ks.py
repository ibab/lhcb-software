### @file
#
# Standard Ks
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Hlt2SharedParticles.GoodParticles import GoodPions
from Configurables import CombineParticles
from HltConf.HltLine import bindMembers, Hlt2Member

__all__ = ( 'KsLL' )

Hlt2SharedKsLL = Hlt2Member( CombineParticles, "KsLL"
                           , DecayDescriptor = "KS0 -> pi+ pi-" 
                           , DaughtersCuts = { "pi+" : "(TRCHI2DOF<20)" } 
                           , CombinationCut = "(ADAMASS('KS0')<50*MeV)"
                           , MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"
                           , InputLocations = [ GoodPions ]
                           )

KsLL = bindMembers( 'Shared', [ GoodPions, Hlt2SharedKsLL ] )
