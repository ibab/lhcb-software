# $Id: Phi2KK.py,v 1.2 2009-08-09 16:30:38 jpalac Exp $
import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import FilterDesktop
#############################
# Standard Unbiased Phi->KK #
#############################

Phi2KK = FilterDesktop('Phi2KK')
Phi2KK.Code = "  (MINTREE('K+'==ABSID, PIDK) > 0.0)" \
              "& (MINTREE('K+'==ABSID, PT) > 500.0)" \
              "& (ADMASS('phi(1020)') < 12.*MeV)" \
              "& (PT > 1000.*MeV)" \
              "& (VFASPF(VCHI2/VDOF) < 20.0)"

from PhysSelPython.Wrappers import Selection, DataOnDemand

MyStdPhi = DataOnDemand('DODStdLoosePhi2KK',
                        Location = 'StdLoosePhi2KK')

SelPhi2KK = Selection("SelPhi2KK",
                      Algorithm = Phi2KK,
                      RequiredSelections = [MyStdPhi] )
