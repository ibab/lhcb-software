"""
Configuration file for 'Standard MC Jpsi2MuMu'.
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"


# =============================================================================
__all__ = (
    'StdMCJpsi2MuMu' ,
    'StdMCpsi2s2MuMu' ,
    'locations'
    )
# =============================================================================


#locations of nopidsparticles: 
npmuons =  "Phys/StdMCMuons/Particles"
#atchmuons = "mcMatch( '[mu+]cc' )"
locations={}
#create matching strings

matchJpsi2MuMu   = "(mcMatch('J/psi(1S) ==> mu+ mu- '))"
matchpsi2s2MuMu   = "(mcMatch('psi(2S) ==> mu+ mu- '))"

# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import CombineParticles
from CommonParticles.Utils import *
from CommonMCParticles import *

## ============================================================================
## create the algorithm
StdMCJpsi2MuMu = CombineParticles ("StdMCJpsi2MuMu")
StdMCJpsi2MuMu.Inputs = [npmuons]
StdMCJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
StdMCJpsi2MuMu.MotherCut = matchJpsi2MuMu
#tdMCJpsi2MuMu.DaughtersCuts = {"mu+" : matchmuons}
StdMCJpsi2MuMu.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCJpsi2MuMu    ))

## create the algorithm
StdMCpsi2s2MuMu = CombineParticles ("StdMCpsi2s2MuMu")
StdMCpsi2s2MuMu.Inputs = [npmuons]
StdMCpsi2s2MuMu.DecayDescriptor = "psi(2S) -> mu+ mu-"
StdMCpsi2s2MuMu.MotherCut = matchpsi2s2MuMu
#tdMCpsi2s2MuMu.DaughtersCuts = {"mu+" : matchmuons}
StdMCpsi2s2MuMu.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCpsi2s2MuMu    ))



## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



