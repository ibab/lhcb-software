#!/usr/bin/env python
# =============================================================================
## @file  CommonParticles/StdTightElectrons.py
#  configuration file for 'Standard Loose Electrons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Tight Electrons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdTightElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from Configurables       import BremAdder

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  CombinedParticleMaker ( 'StdTightElectrons',
                                     Particle =  'electron'  )

# configure the track selector
selector = trackSelector ( algorithm ) 

# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter , 'Electron' )
fltr.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'0.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdTightElectrons = algorithm 

## temporary:
#algorithm.addTool ( BremAdder )
#brem = algorithm.BremAdder 
#brem.BremCor = 1.0

## ============================================================================
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
