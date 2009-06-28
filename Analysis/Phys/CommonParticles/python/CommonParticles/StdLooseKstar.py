#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseKstar.py,v 1.4 2009-06-28 12:02:54 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseKstar.py
#  configuration file for 'Standard Loose Kstar'
#  (based on lifetime unbiased JpsiK* requirements) 
#  @author Greig Cowan
#  @date 2009-06-23
# =============================================================================
"""
Configuration file for 'Standard Loose Kstar'
"""
__author__  = "Greig Cowan"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseKstar2Kpi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseKstar2Kpi = CombineParticles ("StdLooseKstar2Kpi")
StdLooseKstar2Kpi.InputLocations = ["StdLooseKaons", "StdLoosePions"]
StdLooseKstar2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc" 
StdLooseKstar2Kpi.CombinationCut = "(ADAMASS('K*(892)0') < 300.*MeV)"
StdLooseKstar2Kpi.MotherCut = "(VFASPF(VCHI2/VDOF) < 25.) & (PT > 500.*MeV)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseKstar2Kpi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

