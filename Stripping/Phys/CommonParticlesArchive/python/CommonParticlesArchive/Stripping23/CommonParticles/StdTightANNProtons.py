#!/usr/bin/env python
# =============================================================================
# $Id: StdTightANNProtons.py,v 1.4 2009-07-01 18:42:29 jonrob Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightANNProtons.py
#  configuration file for 'Standard Tight ANN Protons' 
#  @author Chris Jones christopher.rob.jones@cern.ch
#  @date 20/01/2012
# =============================================================================
"""
Configuration file for 'Standard Tight ANN Protons'
"""
__author__  = "Chris Jones christopher.rob.jones@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdTightANNProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdTightANNProtons',
                                    Particle = 'proton'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Proton' )
#fltr.Selection = [ "RequiresDet='RICH' ProbNNp>'0.125'" ]
fltr.Selection = [ "ProbNNp>'0.02'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightANNProtons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
