#!/usr/bin/env python
# =============================================================================
# $Id: StdVeryLooseMuons.py,v 1.1 2009-10-08 12:37:06 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdAllVeryLooseMuons.py
#  configuration file for 'Standard All Very Loose Muons' 
#  @author Patrick Koppenburg patrick.koppenburg@cern.ch
#  @date 2009-10-08
# =============================================================================
"""
Configuration file for 'Standard AllVery Loose Muons'
"""
__author__  = "Patrick Koppenburg patrick.koppenburg@cern.ch"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdAllVeryLooseMuons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleMUONFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdAllVeryLooseMuons',
                                     Particle =  'muon'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleMUONFilter, 'Muon' )
fltr.Selection = [ "RequiresDet='MUON' IsMuonLoose=True" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllVeryLooseMuons = algorithm 


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
