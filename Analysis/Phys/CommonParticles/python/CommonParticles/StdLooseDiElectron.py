"""
Configuration file for Standard Loose DiElectron, very loose for penguins
"""

__author__ = 'Patrick Koppenburg'
__date__ = '08/12/2009'


from Gaudi.Configuration import *
from Configurables import CombineParticles 
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDiElectron #
###########################

StdLooseDiElectron = CombineParticles("StdLooseDiElectron")
StdLooseDiElectron.DecayDescriptor = "J/psi(1S) -> e+ e-"
StdLooseDiElectron.InputLocations = ["StdLooseElectrons"]
StdLooseDiElectron.CombinationCut = "AM>100*MeV"
StdLooseDiElectron.MotherCut = "(VFASPF(VCHI2/VDOF)<64)"
locations = updateDoD(StdLooseDiElectron)

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )
    

