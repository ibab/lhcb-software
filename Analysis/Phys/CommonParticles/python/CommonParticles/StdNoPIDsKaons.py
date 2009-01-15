#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsKaons.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsKaons.py
#  configuration file for 'Standard NoPIDs Kaons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Kaons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdNoPIDsKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import PreLoadParticles 
from Configurables         import NoPIDsParticleMaker

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdNoPIDsKaons'          ,
                                DecayDescriptor = 'Kaon' )

# configure particle maker: 
maker  = particleMaker ( algorithm , NoPIDsParticleMaker ) 
maker.Particle = 'kaon'  

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
