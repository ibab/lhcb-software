#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseJpsi2ee.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseJpsi2ee.py
#  configuration file for 'Standard Loose Jpsi2ee' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Jpsi2ee'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseJpsi2ee' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseJpsi2ee = CombineParticles ( 'StdLooseJpsi2ee' )
StdLooseJpsi2ee.InputLocations = [ "StdLooseElectron" ]
StdLooseJpsi2ee.DecayDescriptor = "J/psi(1S) -> e+ e-" 

StdLooseJpsi2ee.CombinationCut = "ADAMASS('J/psi(1S)')<100*MeV"
StdLooseJpsi2ee.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseJpsi2ee )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
