#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseJpsi2MuMu.py,v 1.8 2009-06-29 08:55:36 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseJpsi2MuMu.py
#  configuration file for 'Standard Loose Jpsi2MuMu' 
#  @author Greig Cowan
#  @date 2009-06-23
# =============================================================================
"""
Configuration file for 'Standard Loose Jpsi2MuMu'
"""
__author__  = "Greig Cowan"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
# =============================================================================
__all__ = (
    'StdLooseJpsi2MuMu' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import CombineParticles 
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseJpsi2MuMu = CombineParticles ("StdLooseJpsi2MuMu")
StdLooseJpsi2MuMu.InputLocations = ["StdLooseMuons"]
StdLooseJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 
StdLooseJpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<100*MeV"
StdLooseJpsi2MuMu.MotherCut = "(VFASPF(VCHI2/VDOF) < 25.0)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseJpsi2MuMu    )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 
