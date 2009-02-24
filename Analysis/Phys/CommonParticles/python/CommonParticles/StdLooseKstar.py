#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseKstar.py,v 1.2 2009-02-24 14:19:27 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseKstar.py
#  configuration file for 'Standard Loose ' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Kstar'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseKstar2KPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseKstar2KPi = CombineParticles ( 'StdLooseKstar2KPi' )

StdLooseKstar2KPi.addTool(PhysDesktop)
StdLooseKstar2KPi.PhysDesktop.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
StdLooseKstar2KPi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
StdLooseKstar2KPi.DaughtersCuts = { "pi+" : "(MIPCHI2DV(PRIMARY)>2.25)",
                                    "K+" : "(MIPCHI2DV(PRIMARY)>2.25)" } 
StdLooseKstar2KPi.CombinationCut = "(ADAMASS('K*(892)0')<300)"
StdLooseKstar2KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseKstar2KPi )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
