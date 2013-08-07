"""
Configuration file for 'Standard MC Lambda02pPi'. 
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCLambda02pPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCPions, StandardMCProtons

#locations of nopidsparticles: 

nppions =  "Phys/StdMCPions/Particles"
npprotons =  "Phys/StdMCProtons/Particles"
#matchProtons = "mcMatch( '[p+]cc' )"
#matchPions = "mcMatch( '[pi+]cc' )"

#Create matching strings
matchLambda02pPi  = "(mcMatch('[Lambda0 ==> p+ pi-]CC'))"

## ============================================================================
## create the algorithm
StdMCLambda02pPi = CombineParticles("StdMCLambda02pPi")
StdMCLambda02pPi.Inputs = [nppions,npprotons]
StdMCLambda02pPi.DecayDescriptor = "[Lambda0 -> p+ pi-]cc"
StdMCLambda02pPi.MotherCut = matchLambda02pPi
#StdMCLambda02pPi.DaughtersCuts = {"p+" : matchProtons,"pi+" : matchPions}
StdMCLambda02pPi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations = updateDoD ( StdMCLambda02pPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

