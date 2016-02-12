#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDstarWithD2HH.py,v 1.3 2009-04-22 14:17:39 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDstarWithD02HH.py
#  configuration file for 'Standard Loose Dstar WithD02HH' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Dstar WithD02HH'. Lifetime unbiased at pion level
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $"
# =============================================================================
__all__ = (
    'StdLooseDstarWithD02KPi' ,
    'StdLooseDstarWithD02KPiDCS' ,
    'StdLooseDstarWithD02PiPi' ,
    'StdLooseDstarWithD02KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDstarWithD02KPi = CombineParticles ( 'StdLooseDstarWithD02KPi' )

StdLooseDstarWithD02KPi.Inputs = [ "Phys/StdAllLoosePions/Particles",
                                   "Phys/StdLooseD02KPi/Particles" ]
StdLooseDstarWithD02KPi.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc" 

StdLooseDstarWithD02KPi.CombinationCut = "(ADAMASS('D*(2010)+')<50*MeV) & (APT>1250*MeV)"
StdLooseDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDstarWithD02KPi )

################################################################################
# D* with Cabibbo-suppressed D0->K+Pi- is a clone of D* with D0->KPi
#
StdLooseDstarWithD02KPiDCS = StdLooseDstarWithD02KPi.clone("StdLooseDstarWithD02KPiDCS")
GaudiSequencer("SeqStdLooseDstarWithD02KPiDCS").Members += [ StdLooseDstarWithD02KPiDCS ] 
StdLooseDstarWithD02KPiDCS.Inputs = [ "Phys/StdAllLoosePions/Particles",
                                      "Phys/StdLooseD02KPiDCS/Particles" ]
## configure Data-On-Demand service 
locations.update( updateDoD ( StdLooseDstarWithD02KPiDCS ) )

################################################################################
# D* with D0->KK is a clone of D* with D0->Kpi
#
StdLooseDstarWithD02KK = StdLooseDstarWithD02KPi.clone("StdLooseDstarWithD02KK")
StdLooseDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc"
StdLooseDstarWithD02KK.Inputs = [ "Phys/StdAllLoosePions/Particles",
                                  "Phys/StdLooseD02KK/Particles" ]
## configure Data-On-Demand service 
locations.update( updateDoD ( StdLooseDstarWithD02KK ) )

################################################################################
# D* with D0->PiPi is a clone of D* with D0->KK
#
StdLooseDstarWithD02PiPi = StdLooseDstarWithD02KK.clone("StdLooseDstarWithD02PiPi")
GaudiSequencer("SeqStdLooseDstarWithD02PiPi").Members += [ StdLooseDstarWithD02PiPi ] 
StdLooseDstarWithD02PiPi.Inputs = [ "Phys/StdAllLoosePions/Particles",
                                    "Phys/StdLooseD02PiPi/Particles" ]
## configure Data-On-Demand service 
locations.update( updateDoD ( StdLooseDstarWithD02PiPi ) )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
