#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseElectrons.py,v 1.5 2010-06-01 09:41:12 odescham Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseElectrons.py
#  configuration file for 'Standard Loose Electrons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose Electrons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdAllLooseElectrons',
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombinedParticleMaker 
from Configurables       import ProtoParticleCALOFilter
from CommonParticles.Utils import *

## create the algorithm 
algorithm = CombinedParticleMaker ( 'StdAllLooseElectrons' ,Particle =  'electron'  )
# configure the track selector
selector = trackSelector ( algorithm )
# protoparticle filter:
fltr = protoFilter ( algorithm , ProtoParticleCALOFilter , 'Electron' )
fltr.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'-2.0'" ]
## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdAllLooseElectrons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
