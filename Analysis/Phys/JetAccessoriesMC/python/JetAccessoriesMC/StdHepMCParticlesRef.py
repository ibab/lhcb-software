# =============================================================================
__author__  = " Victor COCO  "
__version__ = ""
# =============================================================================

# =============================================================================
"""
Configuration file for 'Standard HepMC particles for reference (ie all)'
"""

# =============================================================================
__all__ = (
    'StdHepMCParticlesRef' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import  LoKi__HepMCParticleMaker

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  LoKi__HepMCParticleMaker ( 'StdHepMCParticlesRef' ,
                                        PartonicMode = False ,
                                        Particles   = ["gamma","e+","mu+","pi+","K+","nu","p+","nu_mu","nu_tau","n0", "KL0"],
                                        OutputTable = 'Relations/Phys/StdHepMCParticlesRef2HepMC' )

algorithm.MinPtGamma = 0.
algorithm.MinThetaGamma = 0.
algorithm.MaxThetaXGamma = 10.
algorithm.MaxThetaYGamma = 10.
algorithm.MinPCharged = 0.
algorithm.MinPtCharged = 0.
algorithm.MinThetaCharged = 0.
algorithm.MaxThetaCharged = 10.
algorithm.MaxZProduction = 0.
algorithm.MaxRhoProduction = 0.
algorithm.MinZend = 1000000000.
algorithm.MinZend = 1000000000.



# configure the track selector
selector = trackSelector ( algorithm ) 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdHepMCParticlesRef = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================

