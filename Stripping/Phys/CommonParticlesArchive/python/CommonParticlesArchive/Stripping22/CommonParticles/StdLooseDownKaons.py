#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDownKaons.py
# =============================================================================
## @file  CommonParticles/StdLooseDownKaons.py
#  configuration file for 'Standard  Loose Down Kaons' 
#  @author Yasmine Amhis
#  @date 2010-06-03
# =============================================================================
"""
Configuration file for 'Standard Loose Downstream Kaons'
"""
__author__  = "Yasmine Amhis <yasmine.amhis@epfl.ch> "
__version__ = ""
# =============================================================================
__all__ = (
    'StdLooseDownKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ProtoParticleCALOFilter, CombinedParticleMaker

from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker('StdLooseDownKaons',  Particle = 'kaon' )

# configure the track selector
selector = trackSelector ( algorithm , trackTypes = ['Downstream'] ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Kaon' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseDownKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
