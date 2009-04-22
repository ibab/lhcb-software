#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseMuons.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseMuons.py
#  configuration file for 'Standard Loose Muons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Muons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleMUONFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdLooseMuons'          ,
                                DecayDescriptor = 'Muon', Particles = [ 'muon' ] )

# configure the track selector
selector = trackSelector ( algorithm ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleMUONFilter, 'Muon' )
fltr.Selection = [ "RequiresDet='MUON'"  ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseMuons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
