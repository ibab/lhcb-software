#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseElectrons.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseElectrons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import ProtoParticleCALOFilter
from Configurables       import BremAdder

from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PreLoadParticles ( 'StdLooseElectrons'          ,
                                DecayDescriptor = 'Electron' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm )
maker.ExclusiveSelection = False
maker.Particles = [ 'electron' ] 

# configure the track selector
selector = trackSelector ( maker ) 
selector.TrackTypes = [ 'Long' ]

# protoparticle filter:
fltr = protoFilter ( maker , ProtoParticleCALOFilter , 'Electron' )
fltr.Selection = [ "RequiresDet='CALO' CombDLL(e-pi)>'-2.0'" ]

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseElectrons = algorithm 

## temporary:
maker.addTool ( BremAdder )
brem = maker.BremAdder
brem.BremCor = 1.06

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
