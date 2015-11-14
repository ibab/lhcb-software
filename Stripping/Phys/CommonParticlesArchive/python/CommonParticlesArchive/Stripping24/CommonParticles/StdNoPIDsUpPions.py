#!/usr/bin/env python
# =============================================================================
# $Id: StdNoPIDsUpPions.py,v 1.1 2009-10-08 12:37:06 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdNoPIDsUpPions.py
#  configuration file for 'Standard NoPIDs Upstream Pions' 
#  @author Patrick Koppenburg patrick.koppenburg@cern.ch
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard NoPIDs Upstream Pions'
"""
__author__  = "Patrick Koppenburg patrick.koppenburg@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdNoPIDsUpPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import NoPIDsParticleMaker 

from CommonParticles.Utils import *

## create the algorithm 
algorithm = NoPIDsParticleMaker ( 'StdNoPIDsUpPions'          ,
                                  DecayDescriptor = 'Pion' ,
                                  Particle = 'pion' )

# configure the track selector
selector = trackSelector ( algorithm, trackTypes = ['Upstream'] )

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdNoPIDsUpPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
