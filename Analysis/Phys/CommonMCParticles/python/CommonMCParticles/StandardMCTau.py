"""
Configuration file for 'Standard MC Tau'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCTau' ,
    'locations'
    )

# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, FilterDesktop
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCPions
#locations of nopidsparticles: 
nppions =  "Phys/StdMCPions/Particles"
matchPions = "mcMatch( '[pi+]cc' )"
#Create matching strings

matchTau = "(mcMatch('[tau+ ==> pi+ pi- pi+ Nu]CC'))"

## create the algorithm
StdMCTau = CombineParticles("StdMCTau")
StdMCTau.Inputs = [nppions]
StdMCTau.DecayDescriptor = "[tau+ -> pi+ pi- pi+]cc"
StdMCTau.MotherCut = matchTau
#StdMCTau.DaughtersCuts = {"pi+" : matchPions}
StdMCTau.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations = updateDoD ( StdMCTau )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



