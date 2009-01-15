#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseAllPhotons.py,v 1.1 2009-01-15 14:22:14 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseAllPhotons.py
#  configuration file for 'Standard Loose Pions' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose All Photons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseAllPhotons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import CaloParticleMaker
from Configurables       import PhotonMaker

from GaudiKernel.SystemOfUnits import MeV

from CommonParticles.Utils import *


## create the algorithm 
algorithm =  PreLoadParticles ( 'StdLooseAllPhotons'         ,
                                DecayDescriptor = 'Gamma' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm , CaloParticleMaker )
maker.addTool ( PhotonMaker , name = 'PhotonMaker' )
maker.Type = 'PhotonMaker'
photon = maker.PhotonMaker
photon.ConvertedPhotons   = True
photon.UnconvertedPhotons = True 
photon.PtCut              = 200 * MeV 

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseAllPhotons = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
