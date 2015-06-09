#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDipion.py,v 1.8 2012-08-07 14:44:42 elsasser Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDipion.py
#  configuration file for 'Standard Loose Detached Tau' 
#  @author Donal Hill, Conor Fitzpatrick, Christian Elsasser
#  @date 2012-08-07
# =============================================================================
"""
Configuration file for 'Standard Loose Dipion'
"""
__author__  = "Donal Hill, Conor Fitzpatrick, Christian Elsasser"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================
__all__ = (
    'StdLooseDetachedDipion',
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDetachedDipion = CombineParticles ( 'StdLooseDetachedDipion' )

StdLooseDetachedDipion.Inputs = [ "Phys/StdLoosePions/Particles" ]
StdLooseDetachedDipion.DecayDescriptors = ["rho(770)0 -> pi+ pi-",
                                           "[rho(770)+ -> pi+ pi+]cc"]
StdLooseDetachedDipion.DaughtersCuts = {
    "pi+"        : "(PT>150.*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.4) & (PIDK < 8)",
    "pi-"        : "(PT>150.*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.4) & (PIDK < 8)"
    }

#Need an upper mass window of 100 MeV for background estimation
#and a lower mass window of 200 to catch the D+ and Ds

StdLooseDetachedDipion.CombinationCut = "(ANUM(PT < 300*MeV) <= 1) & (ADOCAMAX('') < 0.25*mm) & (AM<1670.*MeV)"
StdLooseDetachedDipion.MotherCut = "(VFASPF(VCHI2) < 9.0) & (MIPCHI2DV(PRIMARY) > 5.0) & (VFASPF(VMINVDDV(PRIMARY)) > 2.0*mm)" 

## configure Data-On-Demand service 
locations = updateDoD (StdLooseDetachedDipion )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
