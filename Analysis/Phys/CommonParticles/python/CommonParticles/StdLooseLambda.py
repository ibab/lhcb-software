#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseLambda.py,v 1.1 2009-10-09 12:39:26 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseLambda.py
#  configuration file for 'Standard Loose ' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Lambda'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseLambdaLL' ,
    'StdLooseLambdaDD' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseLambdaLL = CombineParticles ( 'StdLooseLambdaLL' )

StdLooseLambdaLL.InputLocations = [ "StdLoosePions", "StdLooseProtons" ]
StdLooseLambdaLL.DecayDescriptor = "[Lambda0 -> p+ pi-]cc" 

StdLooseLambdaLL.DaughtersCuts = { "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9) & (TRCHI2DOF<20)",
                                   "p+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>9) & (TRCHI2DOF<20)" } 
StdLooseLambdaLL.CombinationCut = "(ADAMASS('Lambda0')<50*MeV)"
StdLooseLambdaLL.MotherCut = "(ADMASS('Lambda0')<35*MeV) & (VFASPF(VCHI2/VDOF)<30)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseLambdaLL )

## ============================================================================
#  configuration file for 'Standard Loose Downstream ' 
#  @author Greig Cowan
#  @date 2009-04-27
# ============================================================================= 
StdLooseLambdaDD = StdLooseLambdaLL.clone( 'StdLooseLambdaDD' )

StdLooseLambdaDD.InputLocations = [ "StdNoPIDsDownPions", "StdNoPIDsDownProtons" ]

StdLooseLambdaDD.DaughtersCuts = { "pi+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<20)",
                                   "p+" : "(P>2*GeV) & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF<20)" } 
StdLooseLambdaDD.CombinationCut = "(ADAMASS('Lambda0')<80*MeV)"
StdLooseLambdaDD.MotherCut = "(ADMASS('Lambda0')<64*MeV) & (VFASPF(VCHI2/VDOF)<30)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseLambdaDD )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
