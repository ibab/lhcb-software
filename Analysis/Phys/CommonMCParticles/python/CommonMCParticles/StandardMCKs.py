"""
Configuration file for 'Standard MC Ks'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdMCKsLL' ,
    'StdMCKsDD' ,
    'StdMCKsAny' ,
    'locations'
    )

# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, FilterDesktop
from CommonParticles.Utils import *
from CommonMCParticles import StandardMCPions

#locations of nopidsparticles: 

nppions =  "Phys/StdMCPions/Particles"
npdownpions =  "Phys/StdMCDownPions/Particles"
#Create matching strings
locations={}
matchKs = "(mcMatch('[KS0]cc ==> pi+ pi-'))"

## create the algorithm
StdMCKsLL = CombineParticles("StdMCKsLL")
StdMCKsLL.Inputs = [nppions]
StdMCKsLL.DecayDescriptor = "[KS0 -> pi+ pi-]cc"
StdMCKsLL.MotherCut = matchKs
#StdMCKsLL.DaughtersCuts = {"pi+" : matchpions}
StdMCKsLL.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCKsLL ))


## create the algorithm
StdMCKsDD = CombineParticles("StdMCKsDD")
StdMCKsDD.Inputs = [npdownpions]
StdMCKsDD.DecayDescriptor = "[KS0 -> pi+ pi-]cc"
StdMCKsDD.MotherCut = matchKs
#StdMCKsDD.DaughtersCuts = {"pi+" : matchpions}
StdMCKsDD.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCKsDD ))


## create the algorithm
StdMCKsAny = CombineParticles("StdMCKsAny")
StdMCKsAny.Inputs = [nppions, npdownpions]
StdMCKsAny.DecayDescriptor = "[KS0 -> pi+ pi-]cc"
StdMCKsAny.MotherCut = matchKs
#StdMCKsDD.DaughtersCuts = {"pi+" : matchpions}
StdMCKsAny.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

## configure Data-On-Demand service
locations.update(updateDoD ( StdMCKsAny ))


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )



