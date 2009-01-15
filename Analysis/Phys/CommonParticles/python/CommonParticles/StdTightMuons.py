#!/usr/bin/env python
# =============================================================================
# $Id: StdTightMuons.py,v 1.1 2009-01-15 14:22:15 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightMuons.py
#  configuration file for 'Standard Tight Muons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Muons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdTightMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import ProtoParticleMUONFilter


from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdTightMuons'          ,
                                DecayDescriptor = 'Muon' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm )
maker.ExclusiveSelection = False
maker.Particles = [ 'muon' ] 

# configure the track selector
selector = trackSelector ( maker ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( maker , ProtoParticleMUONFilter, 'Muon' )
fltr.Selection = [ "RequiresDet='MUON' CombDLL(mu-pi)>'-3.0'" ] 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightMuons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
