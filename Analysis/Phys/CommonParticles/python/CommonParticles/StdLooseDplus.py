#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDplus.py,v 1.6 2010-08-10 20:55:00 gligorov Exp $ 
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
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $"
# =============================================================================
__all__ = (
    'StdLooseDplus2KPiPi' ,
    'StdLooseDplus2KKPi' ,
    'StdLooseDplus2PiPiPi' ,
    'StdLooseDplus2KPiPiOppSignPi' ,
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

StdLooseDplus2KPiPi.InputLocations = [ "StdNoPIDsKaons", "StdNoPIDsPions" ]
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
StdLooseDplus2PiPiPi.InputLocations = [ "StdNoPIDsPions" ]
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

StdLooseDplus2hhh = CombineParticles ( 'StdLooseDplus2hhh') 
StdLooseDplus2hhh.InputLocations = [ "StdNoPIDsPions" ]
StdLooseDplus2PiPiPi.DecayDescriptor = "[D+ -> pi- pi+ pi+]cc"
StdLooseDplus2KPiPi.DaughtersCuts = {"pi+" : "((TRCHI2DOF<10) & (PT >250*MeV) & (P > 2*GeV) & (MIPCHI2DV(PRIMARY) > 4.))" }
StdLooseDplus2KPiPi.CombinationCut = """
                                    ((in_range(1769*MeV, mpipipi    , 2069 * MeV) | 
                                     (in_range(1769*MeV, mKpipi     , 2069 * MeV) | 
                                     (in_range(1769*MeV, mKKpi      , 2069 * MeV) | 
                                     (in_range(1769*MeV, mKpipiDCS  , 2069 * MeV)
                                    ) & (APT>1.*GeV)
                                    """
StdLooseDplus2KPiPi.MotherCut = "(VFASPF(VCHI2/VDOF) < 10 )"
StdLooseDplus2KPiPi.Preambulo = [   "mpipipi = AWM ('pi-'  , 'pi+' , 'pi+' ) " ,
                                    "mKpipi  = AWM ('K-'   , 'pi+', 'pi+' ) " , 
                                    "mKKpi   = AWM ('K-'   , 'K+' , 'pi+'  ) " , 
                                    "mKpipiDCS  = AWM ( 'pi-' , 'K+' , 'pi+'  )" ]
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
