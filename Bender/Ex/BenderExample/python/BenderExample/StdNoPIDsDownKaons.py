#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownKaons.py,v 1.1 2010-01-12 10:58:34 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsDownPions.py
#  configuration file for 'Standard NoPIDs Downstream Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-01-09
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Downstream Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdNoPIDsDownKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdNoPIDsDownKaons'     ,
                                   DecayDescriptor = 'Kaon' ,
                                   Particle        = 'kaon' )

# configure the track selector
selector = trackSelector ( algorithm, trackTypes = ['Downstream'] )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsDownKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
