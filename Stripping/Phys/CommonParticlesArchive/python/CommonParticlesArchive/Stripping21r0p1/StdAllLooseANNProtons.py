#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdALLLooseANNProtons.py
#  configuration file for 'Standard Loose Protons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose ANN Protons'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdAllLooseANNProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdAllLooseANNProtons',
                                    Particle = 'proton' )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Proton' )
#fltr.Selection = [ "RequiresDet='RICH' ProbNNp>'0.05'" ]
fltr.Selection = [ "ProbNNp>'0.01'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseANNProtons = algorithm

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
