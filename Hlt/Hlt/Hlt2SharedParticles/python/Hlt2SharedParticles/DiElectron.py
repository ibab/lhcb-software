### @file
#
#  Standard Di-electron
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from Configurables import CombineParticles, GaudiSequencer, PhysDesktop
from HltConf.HltLine import bindMembers

__all__ = ( 'DiElectron'  )

Hlt2SharedDiElectron = CombineParticles("Hlt2SharedDiElectron")

Hlt2SharedDiElectron.addTool(PhysDesktop)
Hlt2SharedDiElectron.PhysDesktop.InputLocations = [ "Hlt2Electrons" ]
Hlt2SharedDiElectron.DecayDescriptor = "J/psi(1S) -> e+ e-" 

Hlt2SharedDiElectron.DaughtersCuts = {"e+" : "(PT>800*MeV)"}
Hlt2SharedDiElectron.CombinationCut = "(AM<6*GeV)"
Hlt2SharedDiElectron.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

DiElectron           = bindMembers( None, [ Hlt2SharedDiElectron ] )
