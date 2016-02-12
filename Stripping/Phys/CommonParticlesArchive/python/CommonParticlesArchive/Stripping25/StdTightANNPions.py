#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdTightANNPions.py
#  configuration file for 'Standard Tight Pions' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Tight ANN Pions'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdTightANNPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdTightANNPions',
                                    Particle = 'pion'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Pion' )
fltr.Selection = [ "ProbNNpi>'0.6'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightANNPions = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
