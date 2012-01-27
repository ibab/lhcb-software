#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdLooseKaons.py
#  configuration file for 'Standard Loose Kaons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Kaons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdAllLooseKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ProtoParticleCALOFilter, CombinedParticleMaker
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker('StdAllLooseKaons',  Particle = 'kaon' )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Kaon' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'-5.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
