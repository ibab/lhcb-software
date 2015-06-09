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
Configuration file for 'Standard Loose Kstar'. Lieftime unbiased.
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
StdLooseKstar2Kpi.Inputs = ["Phys/StdAllLooseKaons/Particles",
                            "Phys/StdAllLoosePions/Particles"]
StdLooseKstar2Kpi.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"
StdLooseKstar2Kpi.CombinationCut = "(APT > 500.*MeV) & (ADAMASS('K*(892)0') < 300.*MeV) & (ADOCACHI2CUT(30, ''))"
StdLooseKstar2Kpi.MotherCut = "(VFASPF(VCHI2) < 25.)"

## configure Data-On-Demand service
locations = updateDoD ( StdLooseKstar2Kpi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

