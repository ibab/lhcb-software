#!/usr/bin/env python
# =============================================================================
# $Id: StdVeryLooseDstarWithD2HH.py,v 1.3 2010-01-18 10:08:49 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdVeryLooseDstarWithD02HH.py
#  configuration file for 'Standard VeryLoose Dstar WithD02HH' 
#  similar to  StdLooseDstarWithD2HH.py but using Very Loose D0
#  and also StdNoPIDsUpPions to make the D* slow pion. Add two cuts on slow pion. 
# =============================================================================
"""
Configuration file for 'Standard VeryLoose Dstar WithD02HH'
"""
__author__  = "Marta Calvi"
__date__ = '10/12/2009'
__version__ = "version $Revision: 1.3 $"

# =============================================================================
__all__ = (
    'StdVeryLooseDstarWithD02KPi' ,
    'StdVeryLooseDstarWithD02KPiDCS' ,
    'StdVeryLooseDstarWithD02PiPi' ,
    'StdVeryLooseDstarWithD02KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseDstarWithD02KPi = CombineParticles ( 'StdVeryLooseDstarWithD02KPi' )

StdVeryLooseDstarWithD02KPi.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsUpPions", "StdVeryLooseD02KPi" ]
StdVeryLooseDstarWithD02KPi.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc" 

StdVeryLooseDstarWithD02KPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV) & (APT>1250*MeV)"
StdVeryLooseDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"
StdVeryLooseDstarWithD02KPi.DaughtersCuts = { "pi+" : "(TRCHI2DOF<10) & (PT>110*MeV) & (MIPDV(PRIMARY)>0.04*mm)"}

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KPi )

################################################################################
# D* with Cabibbo-suppressed D0->K+Pi- is a clone of D* with D0->KPi
#
StdVeryLooseDstarWithD02KPiDCS = StdVeryLooseDstarWithD02KPi.clone("StdVeryLooseDstarWithD02KPiDCS")
GaudiSequencer("SeqStdVeryLooseDstarWithD02KPiDCS").Members += [ StdVeryLooseDstarWithD02KPiDCS ] 
StdVeryLooseDstarWithD02KPiDCS.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02KPiDCS.InputLocations = [ "StdNoPIDsPions", "StdNoPIDsUpPions", "StdVeryLooseD02KPiDCS" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KPiDCS )

################################################################################
# D* with D0->KK is a clone of D* with D0->Kpi
#
StdVeryLooseDstarWithD02KK = StdVeryLooseDstarWithD02KPi.clone("StdVeryLooseDstarWithD02KK")
GaudiSequencer("SeqStdVeryLooseDstarWithD02KK").Members += [ StdVeryLooseDstarWithD02KK ] 
StdVeryLooseDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02KK.InputLocations = [  "StdNoPIDsPions", "StdNoPIDsUpPions", "StdVeryLooseD02KK" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KK )

################################################################################
# D* with D0->PiPi is a clone of D* with D0->KK
#
StdVeryLooseDstarWithD02PiPi = StdVeryLooseDstarWithD02KK.clone("StdVeryLooseDstarWithD02PiPi")
GaudiSequencer("SeqStdVeryLooseDstarWithD02PiPi").Members += [ StdVeryLooseDstarWithD02PiPi ] 
StdVeryLooseDstarWithD02PiPi.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02PiPi.InputLocations = [  "StdNoPIDsPions", "StdNoPIDsUpPions", "StdVeryLooseD02PiPi" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02PiPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
