#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownProtons.py,v 1.3 2009-06-12 16:14:29 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsDownProtons.py
#  configuration file for 'Standard NoPIDs Downstream Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Downstream Protons'
"""
__author__  = "Alessio Sarti <Alessio.Sarti@lnf.infn.it>"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdNoPIDsDownProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdNoPIDsDownProtons'         ,
                                DecayDescriptor = 'Proton' ,
                                Particle = 'proton')

# configure the track selector
selector = trackSelector ( algorithm ) 
selector.TrackTypes = [ 'Downstream' ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsDownProtons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
