#!/usr/bin/env python
# =============================================================================
# $Id: StdTightProtons.py,v 1.1 2009-01-15 14:22:15 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightProtons.py
#  configuration file for 'Standard Tight Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Protons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdTightProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import ProtoParticleCALOFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdTightProtons'          ,
                                DecayDescriptor = 'Proton' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm )
maker.ExclusiveSelection = False
maker.Particles = [ 'proton' ] 

# configure the track selector
selector = trackSelector ( maker ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( maker , ProtoParticleCALOFilter, 'Proton' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(p-pi)>'0.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightProtons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
