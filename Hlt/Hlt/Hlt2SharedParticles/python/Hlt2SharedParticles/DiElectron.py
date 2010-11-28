### @file
#
#  Standard Di-electron
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
#OBSOLETE: REDIRECT TO TRACK FITTED ELECTRONS FOR NOW
#SHOULD BE REMOVED ENTIRELY
from Hlt2SharedParticles.TrackFittedDiElectron import TrackFittedDiElectron
__all__ = ('DiElectron')
DiElectron = TrackFittedDiElectron
'''
from Gaudi.Configuration import *
from Configurables import CombineParticles
from Hlt2SharedParticles.BasicParticles import Electrons
from HltLine.HltLine import bindMembers, Hlt2Member

__all__ = ( 'DiElectron'  )

Hlt2SharedDiElectron = Hlt2Member( CombineParticles
                                 , "DiElectron"
                                 , InputLocations = [ Electrons ]
                                 , DecayDescriptor = "J/psi(1S) -> e+ e-" 
                                 , DaughtersCuts = {"e+" : "(PT>300*MeV)"}
                                 , CombinationCut = "AALL"
                                 , MotherCut = "(VFASPF(VCHI2PDOF)<100)"
                                 , WriteP2PVRelations = False
                                 )

DiElectron = bindMembers( "Shared", [ Electrons, Hlt2SharedDiElectron ] )
'''
