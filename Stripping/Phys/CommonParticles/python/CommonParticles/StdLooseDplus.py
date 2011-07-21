#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDplus.py,v 1.8 2010-09-09 08:44:42 jpalac Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDplus.py
#  configuration file for 'Standard Loose Dplus' 
#  @author Vladimir Gligorov
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Dplus'
"""
__author__  = "Vladimir Gligorov"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.8 $"
# =============================================================================
__all__ = (
    'StdLooseDplus2KPiPi' ,
    'StdLooseDplus2KKPi' ,
    'StdLooseDplus2PiPiPi' ,
    'StdLooseDplus2KPiPiOppSignPi' ,
    'StdLooseDsplus2KKPi' ,
    'StdLooseDplus2hhh',
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDplus2KPiPi = CombineParticles ( 'StdLooseDplus2KPiPi' )

StdLooseDplus2KPiPi.Inputs = [ "Phys/StdLooseKaons/Particles",
                               "Phys/StdLoosePions/Particles" ]
StdLooseDplus2KPiPi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc" 
StdLooseDplus2KPiPi.DaughtersCuts = {
    "K+"  : "(P > 2*GeV)",
    "pi+" : "(P > 2*GeV)"
    }

#Need an upper mass window of 100 MeV for background estimation
#and a lower mass window of 200 to catch the D+ and Ds

StdLooseDplus2KPiPi.CombinationCut = "((AM>1760.*MeV) & (AM<2080.*MeV) & ((APT>1.*GeV) | (ASUM(PT)>1.*GeV)) & (ADOCAMAX('')<0.5*mm))"
StdLooseDplus2KPiPi.MotherCut = "((VFASPF(VCHI2) < 30 ) & (M>1770.*MeV) & (M < 2070.*MeV) & (BPVVDCHI2>36) & (BPVDIRA>0.98))" 

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDplus2KPiPi )
##########################################################################################
# D+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2KKPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2KKPi")
StdLooseDplus2KKPi.DecayDescriptor = "[D+ -> K- K+ pi+]cc"
locations = updateDoD ( StdLooseDplus2KKPi )
##########################################################################################
# D+ -> 3 pi is a clone of D+ -> K Pi Pi 
#
StdLooseDplus2PiPiPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2PiPiPi")
StdLooseDplus2PiPiPi.Inputs = [ "Phys/StdLoosePions/Particles" ]
StdLooseDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
locations = updateDoD ( StdLooseDplus2PiPiPi )
##########################################################################################
# D_s+ -> K K Pi is a clone of D+ -> K Pi Pi 
#
StdLooseDsplus2KKPi = StdLooseDplus2KPiPi.clone("StdLooseDsplus2KKPi")
StdLooseDsplus2KKPi.DecayDescriptor = "[D_s+ -> K- K+ pi+]cc"
locations = updateDoD ( StdLooseDsplus2KKPi )
##########################################################################################

# Also need the opposite sign pions mode for the Ds 
#
StdLooseDplus2KPiPiOppSignPi = StdLooseDplus2KPiPi.clone("StdLooseDplus2KPiPiOppSignPi")
StdLooseDplus2KPiPiOppSignPi.DecayDescriptor = "[D+ -> pi- pi+ K+]cc"
locations = updateDoD ( StdLooseDplus2KPiPiOppSignPi )

StdLooseDplus2hhh = CombineParticles ( 'StdLooseDplus2hhh') 
StdLooseDplus2hhh.Inputs = [ "Phys/StdLoosePions/Particles" ]
StdLooseDplus2hhh.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
StdLooseDplus2hhh.DaughtersCuts = {
    "pi+" : "((PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))"
    }

StdLooseDplus2hhh.CombinationCut = """(APT>1.*GeV) & (ADOCACHI2CUT(50, '')) & (
                                      in_range(1769*MeV, mpipipi    , 2069 * MeV) | 
                                      in_range(1769*MeV, mKpipi     , 2069 * MeV) | 
                                      in_range(1769*MeV, mKKpi1     , 2069 * MeV) | 
                                      in_range(1769*MeV, mKKpi2     , 2069 * MeV) | 
                                      in_range(1769*MeV, mKpipiDCS1 , 2069 * MeV) |
                                      in_range(1769*MeV, mKpipiDCS2 , 2069 * MeV)
                                      )
                                    """
StdLooseDplus2hhh.MotherCut = "(VFASPF(VCHI2) < 30 )"
StdLooseDplus2hhh.Preambulo = [   "mpipipi = AWM ('pi-'  , 'pi+', 'pi+' ) " ,
                                  "mKpipi  = AWM ('K-'   , 'pi+', 'pi+' ) " , 
                                  "mKKpi1  = AWM ('K-'   , 'K+' , 'pi+' ) " , 
                                  "mKKpi2  = AWM ('K-'   , 'pi+', 'K+'  ) " , 
                                  "mKpipiDCS1 = AWM ( 'pi-' , 'K+' , 'pi+'  )",
                                  "mKpipiDCS2 = AWM ( 'pi-' , 'pi+', 'K+'   )" ]
## configure Data-On-Demand service
locations = updateDoD (StdLooseDplus2hhh)  

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
