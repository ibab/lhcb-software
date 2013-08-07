"""
Configuration file for 'Standard MC Ks'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdMCKs' ,
    'locations'
    )

# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, FilterDesktop
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCPions

#locations of nopidsparticles: 
nppions =  "Phys/StdMCPions/Particles"
#matchpions = "mcMatch( '[pi+]cc' )"
#Create matching strings

matchKs = "(mcMatch('[KS0]cc ==> pi+ pi-'))"

## create the algorithm
StdMCKs = CombineParticles("StdMCKs")
StdMCKs.Inputs = [nppions]
StdMCKs.DecayDescriptor = "[KS0 -> pi+ pi-]cc"
StdMCKs.MotherCut = matchKs
#StdMCKs.DaughtersCuts = {"pi+" : matchpions}
StdMCKs.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations = updateDoD ( StdMCKs )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



