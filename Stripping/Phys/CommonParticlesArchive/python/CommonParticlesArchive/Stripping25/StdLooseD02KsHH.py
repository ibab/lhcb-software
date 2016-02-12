#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseD02KsHH.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseD02KsHH.py
#  configuration file for 'Standard Loose D0 -> KsHH' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose D0 -> KsHH'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseD02KsPiPi' ,
    'StdLooseD02KsKK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
##########################################################################################
# D0 -> Ks Pi Pi
#
StdLooseD02KsPiPi = CombineParticles("StdLooseD02KsPiPi")

StdLooseD02KsPiPi.Inputs = [ "Phys/StdLoosePions/Particles",
                             "Phys/StdLooseKsLL/Particles" ]

StdLooseD02KsPiPi.DecayDescriptor = "[D0 -> KS0 pi+ pi-]cc" 

StdLooseD02KsPiPi.DaughtersCuts = { "pi+" : "(PT>400*MeV)",
                                      "KS0" : "(PT>1*GeV)"} 
StdLooseD02KsPiPi.CombinationCut = "(ADAMASS('D0')<80*MeV) & (APT>1800*MeV)"
StdLooseD02KsPiPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>1)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KsPiPi )


##########################################################################################
# D0 -> Ks K K is a clone of Ks Pi Pi
#
StdLooseD02KsKK = StdLooseD02KsPiPi.clone("StdLooseD02KsKK")

StdLooseD02KsKK.Inputs = [ "Phys/StdLooseKaons/Particles",
                           "Phys/StdLooseKsLL/Particles" ]
StdLooseD02KsKK.DecayDescriptor = "[D0 -> KS0 K+ K-]cc" 

StdLooseD02KsKK.DaughtersCuts = { "K+" : "(PT>300*MeV)",
                                   "KS0" : "(PT>800*MeV)" } 
StdLooseD02KsKK.CombinationCut = "(ADAMASS('D0')<80) & (APT>1500*MeV)" 
StdLooseD02KsKK.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (MIPCHI2DV(PRIMARY)>0.49)" 

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KsKK )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
