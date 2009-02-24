#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseDs.py,v 1.2 2009-02-24 14:19:26 pkoppenb Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseDs2KKPi.py
#  configuration file for 'Standard Loose Ds2KKPi' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Ds2KKPi'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $"
# =============================================================================
__all__ = (
    'StdLooseDs2KKPi' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles, PhysDesktop
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseDs2KKPi = CombineParticles ( 'StdLooseDs2KKPi' )
StdLooseDs2KKPi.addTool(PhysDesktop)
StdLooseDs2KKPi.PhysDesktop.InputLocations = [ "StdLooseKaons", "StdLoosePions" ]
StdLooseDs2KKPi.DecayDescriptor = "[D_s- -> K+ K- pi-]cc" 
StdLooseDs2KKPi.DaughtersCuts = {"K+"  : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)",
                                 "pi+" : "(PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>4)"}
# need a large window to have the Ds* in as well.
StdLooseDs2KKPi.CombinationCut = "(ADAMASS('D_s-')<200*MeV) & (APT>1*GeV)"
StdLooseDs2KKPi.MotherCut = "(VFASPF(VCHI2/VDOF)<6.67) & (MIPCHI2DV(PRIMARY)>9)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLooseDs2KKPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
