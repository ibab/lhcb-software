#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDplus.py,v 1.4 2009-05-20 18:50:40 gligorov Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDplus.py
#  configuration file for 'Standard Loose Dplus' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Dplus'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
__all__ = (
    'StdLooseDplus2KPiPi' ,
    'StdLooseDplus2KKPi' ,
    'StdLooseDplus2PiPiPi' ,
    'StdLooseDplus2KPiPiOppSignPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDplus2KPiPi = CombineParticles ( 'StdLooseDplus2KPiPi' )

StdLooseDplus2KPiPi.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
StdLooseDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc" 
StdLooseDplus2KPiPi.DaughtersCuts = { "K+"  : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))", "pi+" : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))" }
#Need an upper mass window of 100 MeV for background estimation
#and a lower mass window of 200 to catch the D+ and Ds
StdLooseDplus2KPiPi.CombinationCut = "(ADAMASS('D_s+')<200*MeV) & (APT>1.*GeV)"
StdLooseDplus2KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 ) & (M < 2070.*MeV) " 

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KPiPi )

##########################################################################################
# D+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2KKPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2KKPi")
StdLooseDplus2KKPi.DecayDescriptor = "[D+ -> K- K+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KKPi )
##########################################################################################
# D+ -> 3 pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2PiPiPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2PiPiPi")
StdLooseDplus2PiPiPi.InputLocations = [ "StdLoosePions" ]
StdLooseDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2PiPiPi )
##########################################################################################
# Also need the opposite sign pions mode for the Ds 
#
StdLooseDplus2KPiPiOppSignPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2KPiPiOppSignPi")
StdLooseDplus2KPiPiOppSignPi.DecayDescriptor = "[D+ -> pi- pi+ K+]cc"
## configure Data-On-Demand service
locations = updateDoD ( StdLooseDplus2KPiPiOppSignPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
