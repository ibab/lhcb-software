#!/usr/bin/env python
# =============================================================================
# $Id: StdLoosePions.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLoosePions.py
#  configuration file for 'Standard Loose Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Pions'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLoosePions' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdLoosePions'          ,
                                DecayDescriptor = 'Pion' , Particles = [ 'pion' ] )

# configure the track selector
selector = trackSelector ( algorithm ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter, 'Pion' )
fltr.Selection = [ "" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLoosePions = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
