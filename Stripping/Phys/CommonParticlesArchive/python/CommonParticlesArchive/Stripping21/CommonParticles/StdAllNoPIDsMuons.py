#!/usr/bin/env python
# =============================================================================
# $Id: StdAllNoPIDsMuons.py,v 1.5 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdAllNoPIDsMuons.py
#  configuration file for 'Standard NoPIDs Muons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Muons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdAllNoPIDsMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdAllNoPIDsMuons'  ,
                                   Particle = 'muon'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllNoPIDsMuons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
