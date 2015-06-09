#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdLooseANNUpKaons.py
#  configuration file for 'Standard Loose ANN Upstream Kaons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose Upstream Kaons using ANN PID'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdLooseANNUpKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdLooseANNUpKaons' ,
                                    Particle = 'kaon' )

# configure the track selector
selector = trackSelector ( algorithm,
                           trackTypes = ['Upstream'],
                           cuts = { "Chi2Cut" : [0,10] } )

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Kaon' )
#fltr.Selection = [ "RequiresDet='RICH' ProbNNk>'0.1'" ]
fltr.Selection = [ "ProbNNk>'0.025'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseANNUpKaons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
