#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseAllPhotons.py,v 1.2 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseAllPhotons.py
#  configuration file for 'Standard Loose All Photons' 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2009-01-14
# =============================================================================
"""
Configuration file for 'Standard Loose All Photons'
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseAllPhotons' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PhotonMakerAlg 
from Configurables       import PhotonMaker
from GaudiKernel.SystemOfUnits import MeV
from CommonParticles.Utils import *

## create the algorithm 
algorithm =  PhotonMakerAlg ( 'StdLooseAllPhotons'         ,
                                DecayDescriptor = 'Gamma' )

# configure desktop&particle maker: 
algorithm.addTool ( PhotonMaker , name = 'PhotonMaker' )
photon = algorithm.PhotonMaker
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
