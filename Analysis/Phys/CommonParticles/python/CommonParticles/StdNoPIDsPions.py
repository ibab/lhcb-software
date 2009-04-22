#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsPions.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsPions.py
#  configuration file for 'Standard NoPIDs Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdNoPIDsPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdNoPIDsPions'          ,
                                DecayDescriptor = 'Pion' , Particle = 'pion'  )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
