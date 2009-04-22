#!/usr/bin/env python
# =============================================================================
# $Id: StdTightKaons.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdTightKaons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ProtoParticleCALOFilter, CombinedParticleMaker


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdTightKaons'          ,
                                     DecayDescriptor = 'Kaon' ,
                                     Particles = [ 'kaon' ] )

# configure the track selector
selector = trackSelector ( algorithm ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Kaon' )
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
