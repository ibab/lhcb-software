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
    'StdLoosePi02gg'      ,
    'StdLooseResolvedPi0' ,
    'locations'
    )
# =============================================================================
from GaudiKernel.SystemOfUnits import MeV
from CommonParticles.Utils     import *

## create the algorithm 
from Configurables       import ResolvedPi0Maker, PhotonMaker
wide =  ResolvedPi0Maker ( 'StdLoosePi02gg'           ,
                           DecayDescriptor = 'Pi0'    ,
                           MassWindow      = 60.* MeV )
wide.addTool(PhotonMaker)
wide.PhotonMaker.PtCut = 200.*MeV

## configure Data-On-Demand service 
locations      = updateDoD ( wide  )
## define the symbol:
StdLoosePi02gg = wide 

# ============================================================================
## filter the ``standard''-mass window 
from Configurables       import FilterDesktop 
fltr       =  FilterDesktop (
    'StdLooseResolvedPi0'                   ,
    Code   = "ADMASS('pi0') < 30 * MeV "    ,
    Inputs = [
    'Phys/%s/Particles'  % wide.name() 
    ]
    )
## configure Data-On-Demand service 
locations.update ( updateDoD ( fltr ) ) 
## finally: define the symbol 
StdLooseResolvedPi0 = fltr 

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 
