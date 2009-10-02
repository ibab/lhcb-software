# $Id: Jpsi2MuMu.py,v 1.3 2009-10-02 09:18:25 jpalac Exp $

__author__ = 'Greig Cowan'
__date__ = 'April 2009'
__version__ = '$Revision: 1.3 $'

'''
Roadmap Jpsi->mumu selection using LoKi::Hybrid and python
configurables. This options file is used in the selection of
Bs->JpsiPhi, Bd->JpsiK*, Bu->JpsiK, Bd->JpsiKs.
'''

import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import FilterDesktop

#################
# Make the Jpsi #
#################
Jpsi2MuMu = FilterDesktop('Jpsi2MuMu')
#Jpsi2MuMu.InputLocations =['StdLTUnbiasedJpsi2MuMu']

Jpsi2MuMu.Code = "  (MAXTREE('mu+'==ABSID, TRCHI2DOF) < 5.0)" \
                 "& (MINTREE('mu+'==ABSID, PIDmu) > -5.0)" \
                 "& (MAXTREE('mu+'==ABSID, PIDK) < 5.0)" \
                 "& (ADMASS('J/psi(1S)') < 42.*MeV)"

from PhysSelPython.Wrappers import Selection, DataOnDemand

MyStdJpsi = DataOnDemand('DODStdLTUnbiasedJpsi2MuMu',
                         Location = 'Phys/StdLTUnbiasedJpsi2MuMu')

SelJpsi2MuMu = Selection("SelJpsi2MuMu",
                         Algorithm = Jpsi2MuMu,
                         RequiredSelections = [MyStdJpsi] )
