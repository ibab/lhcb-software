"""
Configuration file for 'Standard MC Phi2KK'. 
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCPhi2KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import *

#locations of nopidsparticles: 
npkaons =  "Phys/StdMCKaons/Particles"
#matchKaons = "mcMatch( '[K+]cc' )"
#Create matching strings

matchPhi2KK  = "(mcMatch('phi(1020) ==> K+ K-'))"


## ============================================================================
## create the algorithm
StdMCPhi2KK = CombineParticles("StdMCPhi2KK")
StdMCPhi2KK.Inputs = [npkaons]
StdMCPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-"
StdMCPhi2KK.MotherCut = matchPhi2KK
#StdMCPhi2KK.DaughtersCuts = {"K+" : matchKaons}
StdMCPhi2KK.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations = updateDoD ( StdMCPhi2KK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

