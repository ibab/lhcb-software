#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseMergedPi0.py,v 1.2 2009-04-03 12:40:32 odescham Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseMergedPi0.py
#  configuration file for 'Standard Loose Pions' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose MergedPi0'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseMergedPi0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import PreLoadParticles 
from Configurables       import CaloParticleMaker
from Configurables       import MergedPi0Maker

from GaudiKernel.SystemOfUnits import MeV

from CommonParticles.Utils import *


## create the algorithm 
algorithm =  PreLoadParticles ( 'StdLooseMergedPi0'         ,
                                DecayDescriptor = 'Pi0' )

# configure desktop&particle maker: 
maker  = particleMaker ( algorithm , CaloParticleMaker )
maker.addTool ( MergedPi0Maker , name = 'MergedPi0Maker' )
maker.Type = 'MergedPi0Maker'
pi0 = maker.MergedPi0Maker
pi0.MassWindow = 60.* MeV

## configure Data-On-Demand service 
locations = updateDoD ( algorithm )

## finally: define the symbol 
StdLooseMergedPi0 = algorithm 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
