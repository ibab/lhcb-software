"""
Configuration file for 'Standard MC Lambdac'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdMCLambdac' ,
    'locations'
    )

# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, FilterDesktop
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKaons, StandardMCPions, StandardMCProtons

#locations of nopidsparticles: 
nppions =  "Phys/StdMCPions/Particles"
npkaons =  "Phys/StdMCKaons/Particles"
npprotons =  "Phys/StdMCProtons/Particles"
#matchKaons = "mcMatch( '[K+]cc' )"
#matchpions = "mcMatch( '[pi+]cc' )"
locations={}
#Create matching strings
matchLambdac = "(mcMatch('[Lambda_c+ ==> p+ K- pi+]CC'))"
## create the algorithm
StdMCLambdac = CombineParticles("StdMCLambdac")
StdMCLambdac.Inputs = [nppions, npkaons, npprotons]
StdMCLambdac.DecayDescriptor = "[Lambda_c+ -> p+ K- pi+]cc"
StdMCLambdac.MotherCut = matchLambdac
#StdMCLambdac.DaughtersCuts = {"K+" : matchKaons, "pi+" : matchpions}
StdMCLambdac.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCLambdac ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



