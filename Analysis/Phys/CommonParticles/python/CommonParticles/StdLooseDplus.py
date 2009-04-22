#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDplus.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseDplus2KPiPi' ,
    'StdLooseDplus2KKPi' ,
    'StdLooseDplus2PiPiPi' ,
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
StdLooseDplus2KPiPi.DaughtersCuts = { "K+"  : "ALL", "pi+" : "ALL" }
#Note that the cut of 300 MeV around the *D_s* mass is NOT a typo, it is motivated by
#the requirements of the B->Dh HLT2 selection
StdLooseDplus2KPiPi.CombinationCut = "(ADAMASS('D_s+')<300*MeV) & (APT>1.5*GeV)"
StdLooseDplus2KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 ) & (BPVDIRA > 0) " 

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KPiPi )

##########################################################################################
# D+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2KKPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2KKPi")
StdLooseDplus2KKPi.DecayDescriptor = "[D+ -> K- K+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KPiPi )
##########################################################################################
# D+ -> 3 pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2PiPiPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2PiPiPi")
StdLooseDplus2PiPiPi.InputLocations = [ "StdLoosePions" ]
StdLooseDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KPiPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
