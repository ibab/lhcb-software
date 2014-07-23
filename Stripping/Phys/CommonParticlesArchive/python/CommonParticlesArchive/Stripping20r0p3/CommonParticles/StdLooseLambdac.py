"""
Configuration file for 'Standard Loose Lambda_c+'
"""
__author__  = "Liming Zhang"
__date__ = '09/12/2009'
__version__ = "version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLooseLambdac2PKPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseLambdac2PKPi = CombineParticles ( 'StdLooseLambdac2PKPi' )

StdLooseLambdac2PKPi.Inputs = [ "Phys/StdLooseKaons/Particles",
                                "Phys/StdLoosePions/Particles",
                                "Phys/StdLooseProtons/Particles" ]

StdLooseLambdac2PKPi.DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc" 
StdLooseLambdac2PKPi.DaughtersCuts = {
    "K+"  : "(P > 2*GeV)", 
    "pi+" : "(P > 2*GeV)",
    "p+"  : "(P > 2*GeV)"
    }

StdLooseLambdac2PKPi.CombinationCut = "((ADAMASS('Lambda_c+')<110*MeV) & (APT>1.*GeV) & (ADOCAMAX('')<0.5*mm))"
StdLooseLambdac2PKPi.MotherCut = "((VFASPF(VCHI2) < 30) & (ADMASS('Lambda_c+')<100*MeV) & (BPVVDCHI2>36) & (BPVDIRA>0.98))" 

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseLambdac2PKPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
