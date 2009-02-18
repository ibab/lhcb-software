#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseD02HH.py,v 1.1 2009-02-18 15:29:29 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseD02HH.py
#  configuration file for 'Standard Loose D0 -> HH' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose D0 -> HH'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseD02KPi' ,
    'StdLooseD02PiPi' ,
    'StdLooseD02KK' ,
    'StdLooseD02KPiDCS' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseD02KPi = CombineParticles ( 'StdLooseD02KPi' )

StdLooseD02KPi.addTool(PhysDesktop())
StdLooseD02KPi.PhysDesktop.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
StdLooseD02KPi.DecayDescriptor = "[D0 -> K- pi+]cc" 

StdLooseD02KPi.DaughtersCuts = { "K+" : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)",
                                   "pi+" : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)"} 
StdLooseD02KPi.CombinationCut = "(ADAMASS('D0')<50) & (APT>1*GeV)"
StdLooseD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KPi )

## ============================================================================
## PiPi
StdLooseD02PiPi = StdLooseD02KPi.clone("StandardLooseD02PiPi")
StdLooseD02PiPi.DecayDescriptor = "[D0 -> pi- pi+]cc"
StdLooseD02PiPi.PhysDesktop.InputLocations = [ "StdLoosePions" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02PiPi )

## ============================================================================
## KK
StdLooseD02KK = StdLooseD02KPi.clone("StandardLooseD02KK")
StdLooseD02KK.DecayDescriptor = "[D0 -> K- K+]cc"
StdLooseD02KK.PhysDesktop.InputLocations = [ "StdLooseKaons" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KK )

## ============================================================================
## Doubly Cabibbo suppressed
StdLooseD02KPiDCS = StdLooseD02KPi.clone("StandardLooseD02KPiDCS")
StdLooseD02KPiDCS.DecayDescriptor = "[D0 -> K+ pi-]cc" 
StdLooseD02KPiDCS.PhysDesktop.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KPiDCS )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
