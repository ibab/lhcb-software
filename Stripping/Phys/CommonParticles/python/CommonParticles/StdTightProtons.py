#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdTightProtons.py
#  configuration file for 'Standard Tight Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Protons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdTightProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdTightProtons', Particle = 'proton'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Proton' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(p-pi)>'0.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightProtons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
