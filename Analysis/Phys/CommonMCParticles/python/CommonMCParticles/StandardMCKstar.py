"""
Configuration file for 'Standard MC Kstar'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdMCKstar' ,
    'locations'
    )

# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, FilterDesktop
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCKaons, StandardMCPions

#locations of nopidsparticles: 
nppions =  "Phys/StdMCPions/Particles"
npkaons =  "Phys/StdMCKaons/Particles"
#matchKaons = "mcMatch( '[K+]cc' )"
#matchpions = "mcMatch( '[pi+]cc' )"
locations={}
#Create matching strings
matchKstar = "(mcMatch('[K*(892)0 ==> K+ pi-]CC'))"

## create the algorithm
StdMCKstar = CombineParticles("StdMCKstar")
StdMCKstar.Inputs = [nppions, npkaons]
StdMCKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StdMCKstar.MotherCut = matchKstar
#StdMCKstar.DaughtersCuts = {"K+" : matchKaons, "pi+" : matchpions}
StdMCKstar.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCKstar ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



