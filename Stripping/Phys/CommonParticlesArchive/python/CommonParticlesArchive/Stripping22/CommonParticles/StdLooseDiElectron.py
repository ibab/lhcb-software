"""
Configuration file for Standard Loose DiElectron, very loose for penguins
"""

__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'


from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

locations = { }

###########################
# StdLooseDiElectron #
###########################
"""
Standard Di-Electron maker from 2 electrons
"""
StdLooseDiElectron = CombineParticles("StdLooseDiElectron")
StdLooseDiElectron.DecayDescriptor = "J/psi(1S) -> e+ e-"
StdLooseDiElectron.Inputs = ["Phys/StdAllLooseElectrons/Particles"]
StdLooseDiElectron.DaughtersCuts = { "e+" : "(PT>500*MeV)" }
StdLooseDiElectron.CombinationCut = "(AM>30*MeV) & (ADOCACHI2CUT(30,''))"
StdLooseDiElectron.MotherCut = "(VFASPF(VCHI2)<25)"
locations.update( updateDoD(StdLooseDiElectron) )

"""
Using Olivier's Di-Electron Particle Maker
"""
###--- From Olivier's ParticleMaker
from Configurables import DiElectronMaker, ProtoParticleCALOFilter
from GaudiKernel.SystemOfUnits import *
dieLL = DiElectronMaker('StdDiElectronFromTracks')
dieLL.Particle = "J/psi(1S)"
selector = trackSelector ( dieLL , trackTypes = ["Long"] ) 
dieLL.addTool( ProtoParticleCALOFilter, name='Electron' )
dieLL.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'-2.0'"]
dieLL.DiElectronMassMax = 5000.*MeV
dieLL.DiElectronMassMin = 0.*MeV
dieLL.DiElectronPtMin = 200.*MeV
locations.update( updateDoD(dieLL) )
StdDiElectronFromTracks = dieLL

# gamma version (clone)
StdDiElectronGamma =  DiElectronMaker('StdDiElectronFromTracks').clone("StdDiElectronGamma")
StdDiElectronGamma.Particle = "gamma"
locations.update(updateDoD( StdDiElectronGamma ))


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )
