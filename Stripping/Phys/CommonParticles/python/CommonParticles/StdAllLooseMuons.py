#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseMuons.py,v 1.7 2009-08-04 09:05:31 jonrob Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $"
# =============================================================================
__all__ = (
    'StdAllLooseMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleMUONFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdAllLooseMuons',
                                     Particle =  'muon'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleMUONFilter, 'Muon' )
fltr.Selection = [ "RequiresDet='MUON' IsMuon=True" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseMuons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
