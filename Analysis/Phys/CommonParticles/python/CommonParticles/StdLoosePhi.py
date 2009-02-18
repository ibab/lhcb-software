#!/usr/bin/env python
# =============================================================================
# $Id: StdLoosePhi.py,v 1.1 2009-02-18 15:29:30 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLoosePhi2KK.py
#  configuration file for 'Standard Loose ' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Phi2KK'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLoosePhi2KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLoosePhi2KK = CombineParticles ( 'StdLoosePhi2KK' )
StdLoosePhi2KK.addTool(PhysDesktop())
StdLoosePhi2KK.PhysDesktop.InputLocations = [ "StdLooseKaons" ]
StdLoosePhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 
StdLoosePhi2KK.DaughtersCuts = { "K+" : "(MIPCHI2DV(PRIMARY)>4)"} 
StdLoosePhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<50*MeV)"
StdLoosePhi2KK.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLoosePhi2KK )
###############################################################################
#
#  Phi for Bs -> Phi Phi selection
#
#  @author N.Styles n.a.styles@ed.ac.uk
#  @date 2008-10-09
#
##
StdLooseUnbiasedPhi2KK = CombineParticles("StdLooseUnbiasedPhi2KK")
StdLooseUnbiasedPhi2KK.addTool(PhysDesktop())
StdLooseUnbiasedPhi2KK.PhysDesktop.InputLocations = [ "StdLooseKaons" ]
StdLooseUnbiasedPhi2KK.DecayDescriptor = "phi(1020) -> K+ K-" 
StdLooseUnbiasedPhi2KK.DaughtersCuts = { "K+" : "(PT>500)" } 
StdLooseUnbiasedPhi2KK.CombinationCut = "(ADAMASS('phi(1020)')<20*MeV)"
StdLooseUnbiasedPhi2KK.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseUnbiasedPhi2KK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
