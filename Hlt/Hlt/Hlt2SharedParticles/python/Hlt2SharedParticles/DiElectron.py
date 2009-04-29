### @file
#
#  Standard Di-electron
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.BasicParticles import Electrons
from HltConf.HltLine import bindMembers, Hlt2Member

__all__ = ( 'DiElectron'  )

Hlt2SharedDiElectron = Hlt2Member( CombineParticles
                                 , "DiElectron"
                                 , InputLocations = [ Electrons ]
                                 , DecayDescriptor = "J/psi(1S) -> e+ e-" 
                                 , DaughtersCuts = {"e+" : "(PT>800*MeV)"}
                                 , CombinationCut = "(AM<6*GeV)"
                                 , MotherCut = "(VFASPF(VCHI2/VDOF)<25)"
                                 )

DiElectron = bindMembers( "Shared", [ Electrons, Hlt2SharedDiElectron ] )
