#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseLambdastar.py
# =============================================================================
## @file  CommonParticles/StdLooseLambdastar.py
#  configuration file for 'Standard Loose Lambdastar'
#  @author Yasmine Amhis
#  @date 2015-05-06
# =============================================================================
"""
Configuration file for 'Standard Loose Lambdastar'. Useful for R_Lambda(*)
"""
__author__  = "Yasmine Amhis"
# =============================================================================
__all__ = (
    'StdLooseLambdastar2pK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *
## ============================================================================
## create the algorithm
StdLooseLambdastar2pK = CombineParticles ("StdLooseLambdastar2pK")
StdLooseLambdastar2pK.Inputs = ["Phys/StdAllLooseKaons/Particles",
                                "Phys/StdAllLooseProtons/Particles"]
StdLooseLambdastar2pK.DecayDescriptor = "[Lambda(1520)0 -> p+ K-]cc"
StdLooseLambdastar2pK.CombinationCut = "(AM < 2600.*MeV) &  (ADOCACHI2CUT(30, ''))"
StdLooseLambdastar2pK.MotherCut = "(VFASPF(VCHI2) < 25.)"

## configure Data-On-Demand service
locations = updateDoD ( StdLooseLambdastar2pK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

