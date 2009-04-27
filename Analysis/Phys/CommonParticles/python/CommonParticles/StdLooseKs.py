#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseKs.py,v 1.4 2009-04-27 14:54:16 gcowan Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseKsLL' ,
    'StdLooseKsDD' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseKsLL = CombineParticles ( 'StdLooseKsLL' )

StdLooseKsLL.InputLocations = [ "StdLoosePions" ]
StdLooseKsLL.DecayDescriptor = "KS0 -> pi+ pi-" 

StdLooseKsLL.DaughtersCuts = { "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9) & (TRCHI2DOF<20)" } 
StdLooseKsLL.CombinationCut = "(ADAMASS('KS0')<50*MeV)"
StdLooseKsLL.MotherCut = "(ADMASS('KS0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseKsLL )

## ============================================================================
#  configuration file for 'Standard Loose Downstream ' 
#  @author Greig Cowan
#  @date 2009-04-27
# ============================================================================= 
StdLooseKsDD = CombineParticles ( 'StdLooseKsDD' )

StdLooseKsDD.InputLocations = [ "StdLoosePions" ]
StdLooseKsDD.DecayDescriptor = "KS0 -> pi+ pi-" 

StdLooseKsDD.DaughtersCuts = { "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<20)" } 
StdLooseKsDD.CombinationCut = "(ADAMASS('KS0')<80*MeV)"
StdLooseKsDD.MotherCut = "(ADMASS('KS0')<64*MeV) & (VFASPF(VCHI2/VDOF)<30)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseKsDD )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
