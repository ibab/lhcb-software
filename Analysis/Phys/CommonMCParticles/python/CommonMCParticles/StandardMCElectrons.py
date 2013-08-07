#!/usr/bin/env python
"""
Configuration file for 'Standard MC Electrons'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *
from CommonParticles.StdNoPIDsElectrons    import *

## create the algorithm 
algorithm = FilterDesktop( 'StdMCElectrons',
                           Inputs = ["Phys/StdNoPIDsElectrons/Particles"],
                           Code = "(mcMatch('[e+]cc'))",
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdMCElectrons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
