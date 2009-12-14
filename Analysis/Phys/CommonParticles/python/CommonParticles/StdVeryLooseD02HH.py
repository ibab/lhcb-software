"""
Configuration file for 'Standard Very Loose D0 -> HH'
"""
__author__  = "Liming Zhang"
__date__ = '09/12/2009'
__version__ = "version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdVeryLooseD02KPi' ,
    'StdVeryLooseD02PiPi' ,
    'StdVeryLooseD02KK' ,
    'StdVeryLooseD02KPiDCS' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseD02KPi = CombineParticles ( 'StdVeryLooseD02KPi' )

StdVeryLooseD02KPi.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsPions" ]
StdVeryLooseD02KPi.DecayDescriptor = "[D0 -> K- pi+]cc" 

StdVeryLooseD02KPi.DaughtersCuts = { "K+" : "(TRCHI2DOF<10) & (PT>250*MeV) & (P>2*GeV) & (MIPDV(PRIMARY)>0.05*mm)",
                                 "pi+" : "(TRCHI2DOF<10) & (PT>250*MeV) & (P>2*GeV) & (MIPDV(PRIMARY)>0.05*mm)"} 
#for the mass window, need 100 MeV above the Ds mass for clean background estimation, but only need 
#~150 MeV below since the lower mass sideband will be for the crystal ball only. 
StdVeryLooseD02KPi.CombinationCut = "(APT>1*GeV) & (ADAMASS('D0')<200*MeV)"
StdVeryLooseD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (M > 1710*MeV)"

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseD02KPi )

## ============================================================================
## PiPi
StdVeryLooseD02PiPi = StdVeryLooseD02KPi.clone("StdVeryLooseD02PiPi")
StdVeryLooseD02PiPi.DecayDescriptor = "[D0 -> pi- pi+]cc"
StdVeryLooseD02PiPi.InputLocations = [ "StdNoPIDsPions" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseD02PiPi )

## ============================================================================
## KK
StdVeryLooseD02KK = StdVeryLooseD02KPi.clone("StdVeryLooseD02KK")
StdVeryLooseD02KK.DecayDescriptor = "[D0 -> K- K+]cc"
StdVeryLooseD02KK.InputLocations = [ "StdNoPIDsKaons" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseD02KK )

## ============================================================================
## Doubly Cabibbo suppressed
StdVeryLooseD02KPiDCS = StdVeryLooseD02KPi.clone("StdVeryLooseD02KPiDCS")
StdVeryLooseD02KPiDCS.DecayDescriptor = "[D0 -> K+ pi-]cc" 
StdVeryLooseD02KPiDCS.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsPions" ]

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseD02KPiDCS )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
