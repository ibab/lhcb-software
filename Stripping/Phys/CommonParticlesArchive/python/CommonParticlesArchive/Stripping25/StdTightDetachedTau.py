#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDetachedTau.py,v 1.0 2012-12-04 14:44:42 elsasser Exp $ 
# =============================================================================
## @file  CommonParticles/StdTightDetachedTau.py
#  configuration file for 'Standard Tight Detached Tau' 
#  @author Donal Hill, Conor Fitzpatrick, Christian Elsasser, Giampiero Mancinelli
#  @date 2012-12-02
# Strongly modified 2012-14-01 by Giampiero Mancinelli
# Used to make the StdLooseDetachedTau3pi in Stripping20r0p3
# Renamed tight afterward (in view of Stripping 21), in order to restore the
# previous loose selection (Sept 9th 2014)
# =============================================================================
"""
Configuration file for 'Standard Tight Detached Tau'
"""
__author__  = "Donal Hill, Conor Fitzpatrick, Christian Elsasser"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.0 $"
# =============================================================================
__all__ = (
           'StdTightDetachedTau3pi',
           'locations'
           
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm - physical tau+ -> pi+ pi- pi+
StdTightDetachedTau3pi = CombineParticles ( 'StdTightDetachedTau3pi' )

StdTightDetachedTau3pi.Inputs = [ "Phys/StdLoosePions/Particles" ]

#Build physical tau and tau+++
StdTightDetachedTau3pi.DecayDescriptors = ["[tau+ -> pi+ pi- pi+]cc"]
StdTightDetachedTau3pi.DaughtersCuts = {
    "pi+"        : "(PT>250.*MeV) & (P>2000.*MeV) & (MIPCHI2DV(PRIMARY) > 16.0) & (TRCHI2DOF<4) & (TRGHOSTPROB<0.4) & (PROBNNpi > 0.55)",
    "pi-"        : "(PT>250.*MeV) & (P>2000.*MeV) & (MIPCHI2DV(PRIMARY) > 16.0) & (TRCHI2DOF<4) & (TRGHOSTPROB<0.4) & (PROBNNpi > 0.55)"
    
    }


StdTightDetachedTau3pi.CombinationCut = "(APT>800.*MeV) & ((AM>400.*MeV) & (AM<2100.*MeV)) & (AMAXDOCA('')<0.2*mm) & (ANUM(PT > 800*MeV) >= 1) "
StdTightDetachedTau3pi.MotherCut = "(PT>1000.*MeV) & (M>500.*MeV) & (M<2000.*MeV) & (BPVDIRA>0.99) & (VFASPF(VCHI2) < 16) & (BPVVDCHI2>16) & (BPVVDRHO>0.1*mm) & (BPVVDRHO<7.0*mm) & (BPVVDZ>5.0*mm)"

## configure Data-On-Demand service 
locations = updateDoD ( StdTightDetachedTau3pi )


## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
