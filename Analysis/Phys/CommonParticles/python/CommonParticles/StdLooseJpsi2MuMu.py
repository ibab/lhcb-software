#!/usr/bin/env python
# =============================================================================
# $Id: StdLooseJpsi2MuMu.py,v 1.5 2009-05-21 12:43:50 ibelyaev Exp $ 
# =============================================================================
## @file  CommonParticles/StdLooseJpsi2MuMu.py
#  configuration file for 'Standard Loose Jpsi2MuMu' 
#  @author Patrick Koppenburg
#  @date 2009-02-18
# =============================================================================
"""
Configuration file for 'Standard Loose Jpsi2MuMu'
"""
__author__  = "Patrick Koppenburg"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $"
# =============================================================================
__all__ = (
    'StdLooseJpsi2MuMu' ,
    'StdUnbiasedJpsi2MuMu' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import CombineParticles, FilterDesktop 
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdLooseJpsi2MuMu = CombineParticles ( 'StdLooseJpsi2MuMu' )
StdLooseJpsi2MuMu.InputLocations = [ "StdLooseMuons" ]
StdLooseJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-" 

StdLooseJpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<100*MeV"
StdLooseJpsi2MuMu.MotherCut = "(VFASPF(VCHI2/VDOF)<25)"

## ============================================================================
#  Filter the loose Jpsi for the unbiased Jpsi selections
#  @author Greig Cowan
#  @date 2009-05-20
StdUnbiasedJpsi2MuMu = FilterDesktop("StdUnbiasedJpsi2MuMu")
StdUnbiasedJpsi2MuMu.InputLocations =["StdLooseJpsi2MuMu"]

StdUnbiasedJpsi2MuMu.Code = "  (MINTREE('mu+'==ABSID, PT) > 500.*MeV)" \
                            "& (MINTREE('mu+'==ABSID, P)  > 0.*MeV)" \
                            "& (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 5.0)" \
                            "& (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \
                            "& (MAXTREE('mu+'==ABSID, PIDK) < 10.0)" \
                            "& (ADMASS('J/psi(1S)') < 50.*MeV)" \
                            "& (PT > 1000.*MeV)" \
                            "& (VFASPF(VCHI2/VDOF) < 6.0)"


## configure Data-On-Demand service 
locations = updateDoD ( StdLooseJpsi2MuMu    )
locations = updateDoD ( StdUnbiasedJpsi2MuMu )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
