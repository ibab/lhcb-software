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
from CommonParticles.Utils     import *
from GaudiKernel.SystemOfUnits import MeV

# ===========================================================
## create the algorithm, eta in wiode mass-window  
from Configurables       import ResolvedPi0Maker, PhotonMaker
wide =  ResolvedPi0Maker (
    'StdLooseEta2gg'            ,
    DecayDescriptor = 'Eta'     ,
    MassWindow      = 105 * MeV ,
    Particle        = 'eta'     )

wide.addTool(PhotonMaker)
wide.PhotonMaker.PtCut  = 200 * MeV
## finally: define the symbol 
StdLooseEtaWide     = wide 
## configure Data-On-Demand service 
locations = updateDoD ( wide )

# ============================================================================
## filter the ``standard''-mass window 
from Configurables       import FilterDesktop 
fltr       =  FilterDesktop (
    'StdLooseResolvedEta'                   ,
    Code   = "ADMASS('eta') < 50 * MeV "    ,
    Inputs = [
    'Phys/%s/Particles'  % wide.name() 
    ]
    )

## configure Data-On-Demand service 
locations.update ( updateDoD ( fltr ) ) 

## finally: define the symbol 
StdLooseResolvedEta = fltr

## ============================================================================
if '__main__' == __name__ :
    
    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
