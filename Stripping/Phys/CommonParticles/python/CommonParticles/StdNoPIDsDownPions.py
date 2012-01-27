#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsDownPions.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsDownPions.py
#  configuration file for 'Standard NoPIDs Downstream Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Downstream Pions'
"""
__author__  = "Alessio Sarti <Alessio.Sarti@lnf.infn.it>"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdNoPIDsDownPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  NoPIDsParticleMaker ( 'StdNoPIDsDownPions'  ,
                                DecayDescriptor = 'Pion' ,
                                Particle = 'pion' )

# configure the track selector
selector = trackSelector ( algorithm,
                           trackTypes = ['Downstream'],
                           cuts = { "Chi2Cut" : [0,10] } )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsDownPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
