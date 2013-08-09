#!/usr/bin/env python
"""
Configuration file for 'Standard MC Pi0'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCMergedPi0' ,
    'StdMCResolvedPi0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *
from CommonParticles import StdLooseMergedPi0, StdLooseResolvedPi0

matchpi0 = "(mcMatch('pi0'))"
locations ={}
## create the algorithm 

StdMCMergedPi0 = FilterDesktop( 'StdMCMergedPi0',
                           Inputs = ["Phys/StdLooseMergedPi0/Particles"],
                           Code = matchpi0,
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)
## configure Data-On-Demand service 
locations.update( updateDoD ( StdMCMergedPi0 ))

StdMCResolvedPi0 = FilterDesktop( 'StdMCResolvedPi0',
                           Inputs = ["Phys/StdLooseResolvedPi0/Particles"],
                           Code = matchpi0,
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)
## configure Data-On-Demand service 
locations.update( updateDoD ( StdMCResolvedPi0 ))

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
