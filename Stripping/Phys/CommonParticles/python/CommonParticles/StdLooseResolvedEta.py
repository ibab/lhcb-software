#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseResolvedEta.py,v 1.1 2010-01-06 11:04:12 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseResolvedEta.py
#  configuration file for 'Standard Loose Resolved Etas' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose ResolvedEta'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseResolvedEta' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ResolvedPi0Maker, PhotonMaker

from GaudiKernel.SystemOfUnits import MeV

from CommonParticles.Utils import *


## create the algorithm 
algorithm =  ResolvedPi0Maker ( 'StdLooseResolvedEta'         ,
                                DecayDescriptor = 'Eta',
                                MassWindow = 50.* MeV,
                                Particle = 'eta')
algorithm.addTool(PhotonMaker)
algorithm.PhotonMaker.PtCut = 200.*MeV

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseResolvedEta = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
