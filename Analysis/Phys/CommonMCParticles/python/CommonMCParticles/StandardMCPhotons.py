#!/usr/bin/env python
"""
Configuration file for 'Standard MC Photons'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCPhotons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *
from CommonParticles import StdLoosePhotons
## create the algorithm 
algorithm = FilterDesktop( 'StdMCPhotons',
                           Inputs = ["Phys/StdLoosePhotons/Particles"],
                           Code = "(mcMatch('gamma'))",
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdMCPhotons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
