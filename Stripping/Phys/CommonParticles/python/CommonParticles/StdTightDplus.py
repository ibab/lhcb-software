#!/usr/bin/env python
## @file  CommonParticles/StdTightDplus.py
#  configuration file for 'Standard Tight Dplus' 
#  @author Sandra Amato
#  @date 2011-08-26# =============================================================================
"""
Configuration file for 'Standard Tight Dplus'"""
__author__  = "Sandra Amato"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
# =============================================================================
__all__ = ( 'StdTightDplus2KPiPiOppSignPi' ,'locations')
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *
## ============================================================================
StdTightDplus2KPiPiOppSignPi = CombineParticles ( 'StdTightDplus2KPiPiOppSignPi')
StdTightDplus2KPiPiOppSignPi.Inputs = [ "Phys/StdLooseKaons/Particles",
                               "Phys/StdLoosePions/Particles" ]

StdTightDplus2KPiPiOppSignPi.DecayDescriptor = "[D+ -> pi- pi+ K+]cc"
StdTightDplus2KPiPiOppSignPi.DaughtersCuts = {
    "K+"  : "(P > 2*GeV) & ((PIDK-PIDpi) > 7.)",
    "pi+" : "(P > 2*GeV)  & ((PIDK-PIDpi) < 3.)"
    }
StdTightDplus2KPiPiOppSignPi.CombinationCut = "((AM>1760.*MeV) & (AM<2080.*MeV) & ((APT>1.*GeV) | (ASUM(PT)>1.*GeV)) & (ADOCAMAX('')<0.5*mm))"
StdTightDplus2KPiPiOppSignPi.MotherCut = "((VFASPF(VCHI2) < 30 ) & (M>1770.*MeV) & (M < 2070.*MeV) & (BPVVDCHI2>36) & (BPVDIRA>0.98))"

locations = updateDoD ( StdTightDplus2KPiPiOppSignPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )

# =============================================================================
# The END 
# =============================================================================
                                                                                            
