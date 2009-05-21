#!/usr/bin/env python
# =============================================================================
# $Id: StdLTUnbiasedJpsi2MuMu.py,v 1.1 2009-05-21 18:59:53 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdLTUnbiasedJpsi2MuMu.py
#  configuration file for 'Standard lifetime unbiased Jpsi2MuMu' 
#  @author Greig Cowan
#  @date 2009-05-21
# =============================================================================
"""
Configuration file for 'Standard lifetime unbiased Jpsi2MuMu'
"""
__author__  = "Greig Cowan"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $"
# =============================================================================
__all__ = (
    'StdLTUnbiasedJpsi2MuMu' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration   import *
from Configurables         import FilterDesktop, CombineParticles 
from CommonParticles.Utils import *

## ============================================================================
StdLTUnbiasedJpsi2MuMu = FilterDesktop("StdLTUnbiasedJpsi2MuMu")
StdLTUnbiasedJpsi2MuMu.InputLocations =["StdLooseJpsi2MuMu"]
StdLTUnbiasedJpsi2MuMu.Code = "  (MINTREE('mu+'==ABSID, PT) > 500.*MeV)" \
                              "& (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
                              "& (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \
                              "& (MAXTREE('mu+'==ABSID, PIDK) < 10.0)" \
                              "& (ADMASS('J/psi(1S)') < 50.*MeV)" \
                              "& (PT > 1000.*MeV)" \
                              "& (VFASPF(VCHI2/VDOF) < 6.0)"

#StdLTUnbiasedJpsi2MuMu = CombineParticles("StdLTUnbiasedJpsi2MuMu")
#StdLTUnbiasedJpsi2MuMu.InputLocations =["StdLooseMuons"]
#StdLTUnbiasedJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
#StdLTUnbiasedJpsi2MuMu.CombinationCut = "ADAMASS('J/psi(1S)')<50*MeV"
#StdLTUnbiasedJpsi2MuMu.MotherCut = "  (MINTREE('mu+'==ABSID, PT) > 500.*MeV)" \
#                                   "& (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 10.0)" \
#                                   "& (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \
#                                   "& (MAXTREE('mu+'==ABSID, PIDK) < 10.0)" \
#                                   "& (VFASPF(VCHI2/VDOF)<25) & (PT > 1000.*MeV)"

## configure Data-On-Demand service 
locations = updateDoD ( StdLTUnbiasedJpsi2MuMu )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
