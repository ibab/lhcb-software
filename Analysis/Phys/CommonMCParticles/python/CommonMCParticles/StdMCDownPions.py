#!/usr/bin/env python
"""
Configuration file for 'Standard MC DownPions'
"""
__author__  = "Conor Fitzpatrick"
__version__ = "1.0"
# =============================================================================
__all__ = (
    'StdMCDownPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import FilterDesktop


from CommonParticles.Utils import *
from CommonParticles.StdNoPIDsDownPions    import *


## create the algorithm 
algorithm = FilterDesktop( 'StdMCDownPions',
                           Inputs = ["Phys/StdNoPIDsDownPions/Particles"],
                           Code = "(mcMatch('[pi+]cc'))",
			   Preambulo = ["from LoKiPhysMC.decorators import *",
			   "from PartProp.Nodes import CC" ]
		)


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdMCDownPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
