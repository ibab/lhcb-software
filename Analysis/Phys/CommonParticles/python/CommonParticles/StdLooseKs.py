#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseKs.py,v 1.2 2009-02-24 14:19:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseKs.py
#  configuration file for 'Standard Loose ' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Ks'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseKsLL' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseKsLL = CombineParticles ( 'StdLooseKsLL' )

StdLooseKsLL = CombineParticles("StdLooseKsLL")

StdLooseKsLL.addTool(PhysDesktop)
StdLooseKsLL.PhysDesktop.InputLocations = [ "StdLoosePions" ]
StdLooseKsLL.DecayDescriptor = "KS0 -> pi+ pi-" 

StdLooseKsLL.DaughtersCuts = { "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9) & (TRCHI2DOF<20)" } 
StdLooseKsLL.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
StdLooseKsLL.MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseKsLL )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
