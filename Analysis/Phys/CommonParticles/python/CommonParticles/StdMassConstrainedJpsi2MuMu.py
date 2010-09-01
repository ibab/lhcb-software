#!/usr/bin/env python
# =============================================================================
# $Id: StdMassConstrainedJpsi2MuMu.py,v 1.1 2010-09-01 14:16:42 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdMassConstrainedJpsi2MuMu.py
#  configuration file for 'Standard mass constrained Jpsi2MuMu'
#  @author Roel Aaij
#  @date 2010-08-31
# =============================================================================
"""
Configuration file for 'Standard mass constrained Jpsi2MuMu'
"""
__author__  = "Roel Aaij"
__version__ = ""
# =============================================================================
__all__ = (
    'StdMassConstrainedJpsi2MuMu' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables import FilterDesktop 
from CommonParticles.Utils import *

## ============================================================================
StdMassConstrainedJpsi2MuMu = FilterDesktop("StdMassConstrainedJpsi2MuMu")
StdMassConstrainedJpsi2MuMu.InputLocations =["StdLooseJpsi2MuMu"]
StdMassConstrainedJpsi2MuMu.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 5.0)" \
                              	   "& (ADMASS('J/psi(1S)') < 80.*MeV)" \
                              	   "& (VFASPF(VCHI2) < 16.)" \
			     	   "& (MFIT)" \
				   "& (INFO( LHCb.Particle.Chi2OfMassConstrainedFit, 100 ) < 25)"
                              	   #"& (MINTREE('mu+'==ABSID, PIDmu) > -10.0)" \

## configure Data-On-Demand service 
locations = updateDoD ( StdMassConstrainedJpsi2MuMu )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 
