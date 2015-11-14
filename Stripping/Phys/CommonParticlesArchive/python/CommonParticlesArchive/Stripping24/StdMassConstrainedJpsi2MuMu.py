#!/usr/bin/env python
# =============================================================================
# $Id: StdMassConstrainedJpsi2MuMu.py,v 1.2 2010-09-05 20:36:44 gcowan Exp $
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
from Configurables import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
StdMassConstrainedJpsi2MuMu = CombineParticles("StdMassConstrainedJpsi2MuMu")
StdMassConstrainedJpsi2MuMu.Inputs = ["Phys/StdAllLooseMuons/Particles"]
StdMassConstrainedJpsi2MuMu.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
StdMassConstrainedJpsi2MuMu.DaughtersCuts = { 'mu+' : '(PIDmu > 0) & (PT > 0.5*GeV)' }
StdMassConstrainedJpsi2MuMu.CombinationCut = "(ADAMASS('J/psi(1S)') < 150.*MeV) & (ADOCACHI2CUT(20, ''))"
StdMassConstrainedJpsi2MuMu.MotherCut = "(VFASPF(VCHI2) < 16.) & (MFIT)"

## configure Data-On-Demand service
locations = updateDoD ( StdMassConstrainedJpsi2MuMu )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations )
