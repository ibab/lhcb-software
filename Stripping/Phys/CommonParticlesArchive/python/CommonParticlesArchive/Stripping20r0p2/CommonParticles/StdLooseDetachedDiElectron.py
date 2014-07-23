"""
Configuration file for Standard Loose Detached DiElectron
"""

__author__ = 'Jibo He'
__date__ = '20/01/2012'


from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

##############################
# StdLooseDetachedDiElectron #
##############################
StdLooseDetachedDiElectron = CombineParticles("StdLooseDetachedDiElectron")
StdLooseDetachedDiElectron.DecayDescriptor = "J/psi(1S) -> e+ e-"
StdLooseDetachedDiElectron.Inputs = ["Phys/StdAllLooseElectrons/Particles"]
StdLooseDetachedDiElectron.DaughtersCuts = { "e+" : "(PT>200*MeV) & (MIPCHI2DV(PRIMARY)>1.)" }
StdLooseDetachedDiElectron.CombinationCut = "(ADOCACHI2CUT(30,''))"
StdLooseDetachedDiElectron.MotherCut = "(VFASPF(VCHI2)<25)"
locations = updateDoD(StdLooseDetachedDiElectron)

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )


