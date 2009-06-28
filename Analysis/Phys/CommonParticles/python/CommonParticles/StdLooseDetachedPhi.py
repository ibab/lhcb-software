##!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDetachedPhi.py,v 1.1 2009-06-28 12:02:54 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDetachedPhi2KK.py
#  configuration file for 'Standard Loose Detached' 
#  @author Greig Cowan
#  @date 2009-06-23
# =============================================================================
"""
Configuration file for 'Standard Loose Detached (i.e., cut on K IP) Phi2KK'
"""
__author__  = "Greig Cowan"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseDetachedPhi2KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDetachedPhi2KK = CombineParticles ("StdLooseDetachedPhi2KK")
StdLooseDetachedPhi2KK.InputLocations = ["StdLooseKaons"]
StdLooseDetachedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 
StdLooseDetachedPhi2KK.DaughtersCuts = {"K+": "(MIPCHI2DV(PRIMARY) > 4.)"} 
StdLooseDetachedPhi2KK.CombinationCut = "(AM < 1100.*MeV)"
StdLooseDetachedPhi2KK.MotherCut = "ALL"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDetachedPhi2KK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 
