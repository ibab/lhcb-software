# =============================================================================
__author__  = " Victor COCO  "
__version__ = ""
# =============================================================================

# =============================================================================
"""
Configuration file for 'Standard HepMC partons'
"""

# =============================================================================
__all__ = (
    'StdHepMCPartons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import  LoKi__ParticleMaker, LoKi__HepMCParticleMaker

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  LoKi__ParticleMaker ( 'StdHepMCPartons' )
algorithm.ParticleMaker = 'LoKi__HepMCParticleMaker'
algorithm.addTool ( LoKi__HepMCParticleMaker )
tool =  getattr ( algorithm , 'LoKi__HepMCParticleMaker' )
tool.PartonicMode = True
tool.OutputTable = 'Relations/Phys/StdHepMCPartons2HepMC'
## leave to the user tool.From = 

# configure the track selector
selector = trackSelector ( algorithm ) 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdHepMCPartons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================

