#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseD02HH.py,v 1.8 2009-07-10 03:05:14 gligorov Exp $
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
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

StdLooseD02KPi.Inputs = [ "Phys/StdLooseKaons/Particles",
                          "Phys/StdLoosePions/Particles" ]
StdLooseD02KPi.DecayDescriptor = "[D0 -> K- pi+]cc"

StdLooseD02KPi.DaughtersCuts = { "K+" : "(P>2*GeV)",
                                 "pi+" : "(P>2*GeV)"}

#for the mass window, need 100 MeV above the Ds mass for clean background estimation, but only need
#~100 MeV below since the lower mass sideband will be for the crystal ball only.

StdLooseD02KPi.CombinationCut = "(((APT>1*GeV) | (ASUM(PT)>1.2*GeV)) & (ADAMASS('D0')<110*MeV) & (ADOCA(1,2)<0.5*mm) & (ADOCACHI2CUT(15,'')))"
StdLooseD02KPi.MotherCut = "((VFASPF(VCHI2)<10) & (ADMASS('D0')<100*MeV) & (BPVVDCHI2>36))"

## configure Data-On-Demand service
locations = updateDoD ( StdLooseD02KPi )

## ============================================================================
## PiPi
StdLooseD02PiPi = StdLooseD02KPi.clone("StdLooseD02PiPi")
StdLooseD02PiPi.DecayDescriptor = "[D0 -> pi- pi+]cc"

## configure Data-On-Demand service
locations.update( updateDoD ( StdLooseD02PiPi ) )

## ============================================================================
## KK
StdLooseD02KK = StdLooseD02KPi.clone("StdLooseD02KK")
StdLooseD02KK.DecayDescriptor = "[D0 -> K- K+]cc"

## configure Data-On-Demand service
locations.update( updateDoD ( StdLooseD02KK ) )

## ============================================================================
## Doubly Cabibbo suppressed
StdLooseD02KPiDCS = StdLooseD02KPi.clone("StdLooseD02KPiDCS")
StdLooseD02KPiDCS.DecayDescriptor = "[D0 -> K+ pi-]cc"

## configure Data-On-Demand service
locations.update( updateDoD ( StdLooseD02KPiDCS ) )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

# =============================================================================
# The END
# =============================================================================
