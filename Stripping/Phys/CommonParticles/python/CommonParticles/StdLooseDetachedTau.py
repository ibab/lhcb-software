#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDetachedTau.py,v 1.0 2012-12-04 14:44:42 elsasser Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDetachedTau.py
#  configuration file for 'Standard Loose Detached Tau' 
#  @author Donal Hill, Conor Fitzpatrick, Christian Elsasser
#  @date 2012-12-02
# =============================================================================
"""
Configuration file for 'Standard Loose Detached Tau'
"""
__author__  = "Donal Hill, Conor Fitzpatrick, Christian Elsasser"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================
__all__ = (
           'StdLooseDetachedTau3pi',
           'locations'
           
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm - physical tau+ -> pi+ pi- pi+
StdLooseDetachedTau3pi = CombineParticles ( 'StdLooseDetachedTau3pi' )

StdLooseDetachedTau3pi.Inputs = [ "Phys/StdLoosePions/Particles" ]

#Build physical tau and tau+++
StdLooseDetachedTau3pi.DecayDescriptors = ["[tau+ -> pi+ pi- pi+]cc"]
StdLooseDetachedTau3pi.DaughtersCuts = {
    "pi+"        : "(PT>150.*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.4) & (PIDK < 8)",
    "pi-"        : "(PT>150.*MeV) & (MIPCHI2DV(PRIMARY) > 4.0) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.4) & (PIDK < 8)"
    
    }

#Need an upper mass window of 100 MeV for background estimation
#and a lower mass window of 200 to catch the D+ and Ds

StdLooseDetachedTau3pi.CombinationCut = "((AM>400.*MeV) & (AM<3500.*MeV)) & (ADOCAMAX('')<0.15*mm) & ((AM12<1670.*MeV) or (AM23<1670.*MeV)) & (ANUM(PT < 300*MeV) <= 1) & (1<ANUM( ('pi+'==ABSID) & (MIPCHI2DV(PRIMARY)>5)))"
StdLooseDetachedTau3pi.MotherCut = "(M>400.*MeV) & (M < 3500.*MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2) < 25 )"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDetachedTau3pi )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
