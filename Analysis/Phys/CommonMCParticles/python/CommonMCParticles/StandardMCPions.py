#!/usr/bin/env python
"""
Configuration file for 'Standard MC Pions'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *

## create the algorithm 
algorithm = FilterDesktop( 'StdMCPions',
                           Inputs = ["Phys/StdNoPIDsPions/Particles"],
                           Code = "(mcMatch('[pi+]cc'))",
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdMCPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
