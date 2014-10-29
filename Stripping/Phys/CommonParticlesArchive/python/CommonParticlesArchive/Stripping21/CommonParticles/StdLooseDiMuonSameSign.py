"""
Configuration file for Standard Loose DiMuon
"""

from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
import GaudiKernel.SystemOfUnits as Units

###########################
# StdLooseDiMuonSameSign  #
###########################

StdLooseDiMuonSameSign = CombineParticles("StdLooseDiMuonSameSign")
StdLooseDiMuonSameSign.DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc"
StdLooseDiMuonSameSign.Inputs = ["Phys/StdAllLooseMuons/Particles"]
StdLooseDiMuonSameSign.CombinationCut="(ADOCACHI2CUT(30,''))"
StdLooseDiMuonSameSign.MotherCut = "(VFASPF(VCHI2) < 25)"
locations = updateDoD(StdLooseDiMuonSameSign)

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )


