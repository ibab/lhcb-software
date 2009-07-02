# $Id: Phi2KK.py,v 1.1 2009-07-02 06:01:15 jpalac Exp $
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import FilterDesktop
#############################
# Standard Unbiased Phi->KK #
#############################

Phi2KK = FilterDesktop('Phi2KK')
Phi2KK.InputLocations =['StdLoosePhi2KK']
Phi2KK.Code = "  (MINTREE('K+'==ABSID, PIDK) > 0.0)" \
              "& (MINTREE('K+'==ABSID, PT) > 500.0)" \
              "& (ADMASS('phi(1020)') < 12.*MeV)" \
              "& (PT > 1000.*MeV)" \
              "& (VFASPF(VCHI2/VDOF) < 20.0)"

