"""
Configuration file for 'Standard VeryLoose Lambda_c+'
"""
__author__  = "Liming Zhang"
__date__ = '09/12/2009'
__version__ = "version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdVeryLooseLambdac2PKPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseLambdac2PKPi = CombineParticles ( 'StdVeryLooseLambdac2PKPi' )

StdVeryLooseLambdac2PKPi.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsPions", "StdNoPIDsProtons" ]
StdVeryLooseLambdac2PKPi.DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc" 
StdVeryLooseLambdac2PKPi.DaughtersCuts = { "K+"  : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>0.05*mm))", 
                                           "pi+" : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>0.05*mm))",
                                           "p+"  : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPDV(PRIMARY)>0.05*mm))"
                                          }
StdVeryLooseLambdac2PKPi.CombinationCut = "(ADAMASS('Lambda_c+')<150*MeV) & (APT>1.*GeV)"
StdVeryLooseLambdac2PKPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 )" 

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseLambdac2PKPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
