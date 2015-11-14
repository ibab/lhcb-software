#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDetachedTau.py,v 1.0 2012-12-04 14:44:42 elsasser Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDetachedTau.py
#  configuration file for 'Standard Loose Detached Tau' 
#  @author Donal Hill, Conor Fitzpatrick, Christian Elsasser
#  @date 2012-08-07
# =============================================================================
"""
Configuration file for 'Standard Loose Detached Tau Non Physical Tau'
"""
__author__  = "Donal Hill, Conor Fitzpatrick, Christian Elsasser"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================
__all__ = (
           'StdLooseDetachedTau3piNonPhys',
           'locations'
           
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm - non-physical tau+++ -> pi+ pi+ pi+
StdLooseDetachedTau3piNonPhys = CombineParticles ( 'StdLooseDetachedTau3piNonPhys' )


StdLooseDetachedTau3piNonPhys.Inputs = [ "Phys/StdLoosePions/Particles" ]

#Build physical tau and tau+++
StdLooseDetachedTau3piNonPhys.DecayDescriptors = ["[tau+ -> pi+ pi+ pi+]cc"]
StdLooseDetachedTau3piNonPhys.DaughtersCuts = {
    "pi+"        : "(PT>150.*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRCHI2DOF<4) & (TRGHOSTPROB<0.4) & (PIDK < 8)"
    }

#Need an upper mass window of 100 MeV for background estimation
#and a lower mass window of 200 to catch the D+ and Ds

StdLooseDetachedTau3piNonPhys.CombinationCut = "((AM>400.*MeV) & (AM<3500.*MeV)) & (ADOCAMAX('')<0.15*mm) & ((AM12<1670.*MeV) or (AM23<1670.*MeV)) & (ANUM(PT < 300*MeV) <= 1) & (1<ANUM( ('pi+'==ABSID) & (MIPCHI2DV(PRIMARY)>5)))"
StdLooseDetachedTau3piNonPhys.MotherCut = "(M>400.*MeV) & (M < 3500.*MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2) < 25 )"



## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDetachedTau3piNonPhys )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

# =============================================================================
# The END 
# =============================================================================
