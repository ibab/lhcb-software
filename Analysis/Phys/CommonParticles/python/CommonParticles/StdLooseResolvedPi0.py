#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseResolvedPi0.py,v 1.2 2009-04-03 12:40:32 odescham Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseResolvedPi0.py
#  configuration file for 'Standard Loose Pions' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose ResolvedPi0'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseResolvedPi0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import CaloParticleMaker
from Configurables       import ResolvedPi0Maker, PhotonMaker

from GaudiKernel.SystemOfUnits import MeV

from CommonParticles.Utils import *


## create the algorithm 
algorithm =  PreLoadParticles ( 'StdLooseResolvedPi0'         ,
                                DecayDescriptor = 'Pi0' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm , CaloParticleMaker )
maker.addTool ( ResolvedPi0Maker , name = 'ResolvedPi0Maker' )
maker.Type = 'ResolvedPi0Maker'
pi0 = maker.ResolvedPi0Maker
pi0.MassWindow = 30.* MeV
pi0.addTool(PhotonMaker())
pi0.PhotonMaker.PtCut = 200.*MeV

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
