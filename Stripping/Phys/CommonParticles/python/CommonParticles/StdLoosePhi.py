##!/usr/bin/env python
# =============================================================================
# $Id: StdLoosePhi.py,v 1.6 2009-06-29 09:16:49 gcowan Exp $
# =============================================================================
## @file  CommonParticles/StdLoosePhi2KK.py
#  configuration file for 'Standard Loose Phi -> KK'
#  @author Greig Cowan
#  @date 2009-06-23
# =============================================================================
"""
Configuration file for 'Standard Loose Phi2KK'. Lieftime unbiased.
"""
__author__  = "Greig Cowan"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $"
# =============================================================================
__all__ = (
    'StdLoosePhi2KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm
StdLoosePhi2KK = CombineParticles("StdLoosePhi2KK")
StdLoosePhi2KK.Inputs = ["Phys/StdAllLooseKaons/Particles"]
StdLoosePhi2KK.DecayDescriptor = "phi(1020) -> K+ K-"
StdLoosePhi2KK.CombinationCut = "(AM < 1100.*MeV) & (ADOCACHI2CUT(30, ''))"
StdLoosePhi2KK.MotherCut = "(VFASPF(VCHI2) < 25.0)"

## configure Data-On-Demand service
locations = updateDoD ( StdLoosePhi2KK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )
