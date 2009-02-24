#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseRho0.py,v 1.2 2009-02-24 14:19:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseRho0.py
#  configuration file for 'Standard Loose Rho0' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose '
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseRho0' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseRho0 = CombineParticles ( 'StdLooseRho0' )

StdLooseRho0.addTool(PhysDesktop)
StdLooseRho0.PhysDesktop.InputLocations  = [ "StdLoosePions" ]
StdLooseRho0.DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc"
StdLooseRho0.DaughtersCuts =  { "pi+" : "TRUE"  }
StdLooseRho0.CombinationCut = "(ADAMASS('rho(770)0')<100*MeV)"
StdLooseRho0.MotherCut = "(BPVVDZ>0) & (VFASPF(VCHI2/VDOF)<9) & (BPVDIRA>0.95) & (BPVVDCHI2>25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseRho0 )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
