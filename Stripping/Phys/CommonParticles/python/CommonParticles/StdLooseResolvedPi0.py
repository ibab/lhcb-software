#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseResolvedPi0.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseResolvedPi0.py
#  configuration file for 'Standard Loose Resolved Pi0s' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose ResolvedPi0'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseResolvedPi0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import ResolvedPi0Maker, PhotonMaker

from GaudiKernel.SystemOfUnits import MeV

from CommonParticles.Utils import *


## create the algorithm 
algorithm =  ResolvedPi0Maker ( 'StdLooseResolvedPi0'         ,
                                DecayDescriptor = 'Pi0',
                                MassWindow = 30.* MeV )
algorithm.addTool(PhotonMaker)
algorithm.PhotonMaker.PtCut = 200.*MeV

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseResolvedPi0 = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
