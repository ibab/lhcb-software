#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsMuons.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsMuons.py
#  configuration file for 'Standard NoPIDs Muons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Muons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdNoPIDsMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdNoPIDsMuons'          ,
                                DecayDescriptor = 'Muon' , Particle = 'muon'  )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsMuons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
