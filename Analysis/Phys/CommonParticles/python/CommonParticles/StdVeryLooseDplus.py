"""
Configuration file for 'Standard VeryLoose Dplus'
"""
__author__  = "Liming Zhang"
__date__ = '09/12/2009'
__version__ = "version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdVeryLooseDplus2KPiPi' ,
    'StdVeryLooseDplus2KKPi' ,
    'StdVeryLooseDplus2PiPiPi' ,
    'StdVeryLooseDplus2KPiPiOppSignPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseDplus2KPiPi = CombineParticles ( 'StdVeryLooseDplus2KPiPi' )

StdVeryLooseDplus2KPiPi.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsPions" ]
StdVeryLooseDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc" 
StdVeryLooseDplus2KPiPi.DaughtersCuts = { "K+"  : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>0.05*mm))", "pi+" : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>0.05*mm))" }
#Need an upper mass window of 150 MeV for background estimation
#and a lower mass window of 250 to catch the D+ and Ds
StdVeryLooseDplus2KPiPi.CombinationCut = "(ADAMASS('D_s+')<250*MeV) & (APT>1.*GeV)"
StdVeryLooseDplus2KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 ) & (M < 2120.*MeV) " 

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDplus2KPiPi )

##########################################################################################
# D+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
StdVeryLooseDplus2KKPi = StdVeryLooseDplus2KPiPi.clone("StdVeryLooseDplus2KKPi")
StdVeryLooseDplus2KKPi.DecayDescriptor = "[D+ -> K- K+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDplus2KKPi )
##########################################################################################
# D+ -> 3 pi is a clone of D+ -> K Pi Pi 
#
StdVeryLooseDplus2PiPiPi = StdVeryLooseDplus2KPiPi.clone("StdVeryLooseDplus2PiPiPi")
StdVeryLooseDplus2PiPiPi.InputLocations = [ "StdNoPIDsPions" ]
StdVeryLooseDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDplus2PiPiPi )
##########################################################################################
# Also need the opposite sign pions mode for the Ds 
#
StdVeryLooseDplus2KPiPiOppSignPi = StdVeryLooseDplus2KPiPi.clone("StdVeryLooseDplus2KPiPiOppSignPi")
StdVeryLooseDplus2KPiPiOppSignPi.DecayDescriptor = "[D+ -> pi- pi+ K+]cc"
## configure Data-On-Demand service
locations = updateDoD ( StdVeryLooseDplus2KPiPiOppSignPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
