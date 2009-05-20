#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseD02HH.py,v 1.5 2009-05-20 18:50:32 gligorov Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
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
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseD02KPi = CombineParticles ( 'StdLooseD02KPi' )

StdLooseD02KPi.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
StdLooseD02KPi.DecayDescriptor = "[D0 -> K- pi+]cc" 

StdLooseD02KPi.DaughtersCuts = { "K+" : "(TRCHI2DOF<10) & (PT>250*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)",
                                 "pi+" : "(TRCHI2DOF<10) & (PT>250*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)"} 
#for the mass window, need 100 MeV above the Ds mass for clean background estimation, but only need 
#~60 MeV below since the lower mass sideband will be for the crystal ball only. 
StdLooseD02KPi.CombinationCut = "(APT>1*GeV) & (ADAMASS('D0')<200*MeV)"
StdLooseD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (M > 1800*MeV)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KPi )

## ============================================================================
## PiPi
StdLooseD02PiPi = StdLooseD02KPi.clone("StdLooseD02PiPi")
StdLooseD02PiPi.DecayDescriptor = "[D0 -> pi- pi+]cc"
StdLooseD02PiPi.InputLocations = [ "StdLoosePions" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02PiPi )

## ============================================================================
## KK
StdLooseD02KK = StdLooseD02KPi.clone("StdLooseD02KK")
StdLooseD02KK.DecayDescriptor = "[D0 -> K- K+]cc"
StdLooseD02KK.InputLocations = [ "StdLooseKaons" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseD02KK )

## ============================================================================
## Doubly Cabibbo suppressed
StdLooseD02KPiDCS = StdLooseD02KPi.clone("StdLooseD02KPiDCS")
StdLooseD02KPiDCS.DecayDescriptor = "[D0 -> K+ pi-]cc" 
StdLooseD02KPiDCS.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]

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
