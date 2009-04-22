#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseJpsi2MuMu.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseJpsi2MuMu.py
#  configuration file for 'Standard Loose Jpsi2MuMu' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Jpsi2MuMu'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseJpsi2MuMu' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseJpsi2MuMu = CombineParticles ( 'StdLooseJpsi2MuMu' )
StdLooseJpsi2MuMu.InputLocations = [ "StdLooseMuons" ]
StdLooseJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 

StdLooseJpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<100*MeV"
StdLooseJpsi2MuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseJpsi2MuMu )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
