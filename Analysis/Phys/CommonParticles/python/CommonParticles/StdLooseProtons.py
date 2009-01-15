#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseProtons.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseProtons.py
#  configuration file for 'Standard Loose Protons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Protons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseProtons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import ProtoParticleCALOFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdLooseProtons'          ,
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
fltr.Selection = [ "RequiresDet='RICH' CombDLL(p-pi)>'-5.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseProtons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
