#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdLooseANNKaons.py
#  configuration file for 'Standard Loose ANN Kaons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose ANN Kaons'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdAllLooseANNKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ProtoParticleCALOFilter, CombinedParticleMaker
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker( 'StdAllLooseANNKaons',
                                   Particle = 'kaon' )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Kaon' )
#fltr.Selection = [ "RequiresDet='RICH' ProbNNk>'0.075'" ]
fltr.Selection = [ "ProbNNk>'0.02'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseANNKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
