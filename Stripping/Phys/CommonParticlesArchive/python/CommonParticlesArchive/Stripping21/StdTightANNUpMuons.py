#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdTightANNUpMuons.py
#  configuration file for 'Standard Loose ANN Upstream Muons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Loose Upstream Muons using ANN PID'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdTightANNUpMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleMUONFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdTightANNUpMuons' ,
                                    Particle = 'muons' )

# configure the track selector
selector = trackSelector ( algorithm,
                           trackTypes = ['Upstream'],
                           cuts = { "Chi2Cut" : [0,10] } )

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleMUONFilter, 'Muon' )
#fltr.Selection = [ "RequiresDet='RICH' ProbNNmu>'0.1'" ]
fltr.Selection = [ "ProbNNmu>'0.1'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightANNUpMuons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
