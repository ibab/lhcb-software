#!/usr/bin/env python
# =============================================================================
# $Id: StdTightKaons.py,v 1.1 2009-01-15 14:22:15 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightKaons.py
#  configuration file for 'Standard Tight Kaons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Kaons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdTightKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import ProtoParticleCALOFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdTightKaons'          ,
                                DecayDescriptor = 'Kaon' )

# configure desktop&particle maker:
maker  = particleMaker ( algorithm )
maker.ExclusiveSelection = False
maker.Particles = [ 'kaon' ] 

# configure the track selector
selector = trackSelector ( maker ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( maker , ProtoParticleCALOFilter, 'Kaon' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(k-pi)>'0.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightKaons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
