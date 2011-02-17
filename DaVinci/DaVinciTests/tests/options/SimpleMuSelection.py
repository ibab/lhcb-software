"""
  @ Created by R. LAmbert
  @ date 2009-11-20
  Mu selection for tests
  Long track muons with PT > 1 GeV
"""

__author__ = 'R. Lambert'
__date__ = 'November 2009'
__version__ = '$Revision: 1.3 $'

import GaudiKernel.SystemOfUnits as Units
from Gaudi.Configuration import *
from Configurables import FilterDesktop, DaVinci
from PhysSelPython.Wrappers import Selection, DataOnDemand, SelectionSequence
from StandardParticles import StdLooseMuons as MyStdMuons

MuForTests = FilterDesktop("_bachelorMu")
MuForTests.Code = "((ISLONG) & (PT > 250.*MeV))"



SelMuForTests = Selection("BachelorMuForTests",
                          Algorithm=MuForTests,
                          RequiredSelections = [MyStdMuons])
from Configurables import CheckPV
checkPV = CheckPV('TestCheckPV')
TestSequence = SelectionSequence("TestSeq",
                                 TopSelection = SelMuForTests,
                                 EventPreSelector = [checkPV])

dv=DaVinci()
DaVinci().UserAlgorithms=[ TestSequence.sequence() ]
