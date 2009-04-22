#!/usr/bin/env python
# =============================================================================
# $Id: StdTightPions.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightPions.py
#  configuration file for 'Standard Tight Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdTightPions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdTightPions'          ,
                                DecayDescriptor = 'Pion', Particles = [ 'pion' ] )

# configure the track selector
selector = trackSelector ( algorithm ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Pion' )
fltr.Selection = [ "RequiresDet='RICH' CombDLL(pi-k)>'-5.0'" ]


## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightPions = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
