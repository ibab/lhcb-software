# $Id: Phi2KK.py,v 1.4 2010-08-02 13:13:01 jpalac Exp $
from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
#############################
# Standard Unbiased Phi->KK #
#############################

Phi2KK = FilterDesktop()
Phi2KK.Code = "  (MINTREE('K+'==ABSID, PIDK) > 0.0)" \
              "& (MINTREE('K+'==ABSID, PT) > 500.0)" \
              "& (ADMASS('phi(1020)') < 12.*MeV)" \
              "& (PT > 1000.*MeV)" \
              "& (VFASPF(VCHI2/VDOF) < 20.0)"

from PhysSelPython.Wrappers import Selection, DataOnDemand

MyStdPhi = DataOnDemand(Location = 'Phys/StdLoosePhi2KK/Particles')

SelPhi2KK = Selection("SelPhi2KK",
                      Algorithm = Phi2KK,
                      RequiredSelections = [MyStdPhi] )
