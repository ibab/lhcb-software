#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__email__ = 'akozlins@cern.ch'
__date__ = '31/08/2010'
__version__ = '$Revision: 1.2 $'

"""

Stripping lines for prompt D* and D0 with D0 -> mu mu.
Control channels: D0 -> pi+ pi-, K+ K-, K- pi+, K+ pi-

Cuts:
  * Particles input: StdLooseMuons, StdNoPIDsPions, StdNoPIDsKaons
  * Track-Chi2/NDOF of the D0 daughters < 5
  * Pt of the D0 daughters > 1 GeV
  * D0 mass window 50 MeV
  * Chi2 of the D0 vertex < 25
  * Chi2 of the D0 life time fit < 36
  * D0 life time > 0.1 ps
  * Track-Chi2/NDOF of the soft pion from D* < 5
  * M(D*) - M(D0) < 155 MeV
  * Chi2 of the D* vertex < 25
  * HLT = 'Hlt.*(MuMu|MBMicro|Muon).*Decision'

RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py:
--------------------------------------------------------------------------------------------
Decision name                                      :     Rate   Accepted   Mult.  <T>,ms
--------------------------------------------------------------------------------------------
StrippingGlobal                                    : 0.000564         15           1.003
--------------------------------------------------------------------------------------------
StrippingSequenceStreamTest                        : 0.000564         15           0.997
-- StrippingD02MuMu                                : 0.000601         16   0.625   0.256
-- StrippingDstarWithD02MuMu                       : 0.000451         12   0.083   0.117
-- StrippingD02HHForD02MuMu                        : 0.000714         19   1.368   0.035
-- StrippingDstarWithD02HHForD02MuMu               : 0.000488         13   0.154   0.034
--------------------------------------------------------------------------------------------
"""

D0DaughtersCutsMuons = \
  "(TRCHI2DOF < 5) & (PT > 1 * GeV)"
D0DaughtersCutsPion = \
  "(TRCHI2DOF < 5) & (PT > 1 * GeV)"
D0DaughtersCutsKaon = \
  "(TRCHI2DOF < 5) & (PT > 1 * GeV)"
D0CombinationCut = \
  "(ADAMASS('D0') < 60 * MeV)"
D0MotherCut = \
  "(VFASPF(VCHI2) < 25) & " + \
  "(ADMASS('D0') < 50 * MeV) & " + \
  "(BPVLTFITCHI2() < 36) & " + \
  "(BPVLTIME() > 0.1 * ps)"

DstarDaughtersCutsPion = \
  "(TRCHI2DOF < 5)"
DstarCombinationCut = \
  "AM - AM1 < 160 * MeV"
DstarMotherCut = \
  "(VFASPF(VCHI2) < 25) & " + \
  "((M - M1) < 155 * MeV)"

from PhysSelPython.Wrappers import DataOnDemand
muons = DataOnDemand(Location = "Phys/StdLooseMuons")
pions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
kaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

# D0 combine
from Configurables import CombineParticles
D0Combine = CombineParticles( "CombD02MuMu" )
D0Combine.DecayDescriptors = [ "[D0 -> mu+ mu-]cc", "[D0 -> pi+ pi-]cc", "[D0 -> K- pi+]cc", "[D0 -> K+ pi-]cc", "[D0 -> K+ K-]cc" ]
D0Combine.DaughtersCuts = { "mu+" : D0DaughtersCutsMuons, "pi+" : D0DaughtersCutsPion, "K+" : D0DaughtersCutsKaon }
D0Combine.CombinationCut = D0CombinationCut
D0Combine.MotherCut = D0MotherCut

# D* combine
DstarCombine = CombineParticles( "CombDstarWithD02MuMu" )
DstarCombine.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
DstarCombine.DaughtersCuts = { "pi+" : DstarDaughtersCutsPion }
DstarCombine.CombinationCut = DstarCombinationCut
DstarCombine.MotherCut = DstarMotherCut

# D0 and D* selections
from PhysSelPython.Wrappers import Selection
D02MuMuSelection = Selection("SelD02MuMu", Algorithm = D0Combine, RequiredSelections = [ muons ])
DstarWithD02MuMuSelection = Selection("SelDstarWithD02MuMu", Algorithm = DstarCombine, RequiredSelections = [ muons, pions, D02MuMuSelection ])
D02HHSelection = Selection("SelD02HH", Algorithm = D0Combine, RequiredSelections = [ pions, kaons ])
DstarWithD02HHSelection = Selection("SelDstarWithD02HH", Algorithm = DstarCombine, RequiredSelections = [ pions, kaons, D02HHSelection ])

# D* stripping line
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__VoidFilter

lineD02MuMu = StrippingLine("D02MuMu",
  algos = [ D02MuMuSelection ],
  prescale = 1,
  HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

lineDstarWithD02MuMu = StrippingLine("DstarWithD02MuMu",
  algos = [ DstarWithD02MuMuSelection ],
  prescale = 1,
  HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

lineD02HHForD02MuMu = StrippingLine("D02HHForD02MuMu",
  algos = [ D02HHSelection ],
  prescale = 0.005,
  HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

lineDstarWithD02HHForD02MuMu = StrippingLine("DstarWithD02HHForD02MuMu",
  algos = [ DstarWithD02HHSelection ],
  prescale = 0.005,
  HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

lines = [
  lineD02MuMu,
  lineDstarWithD02MuMu,
  lineD02HHForD02MuMu,
  lineDstarWithD02HHForD02MuMu
]

if '__main__' == __name__ :

  print __doc__
  print 'Output location ([D0 -> mu+ mu-]cc): ' , lineD02MuMu.outputLocation()
  print 'Output selection [(D0 -> mu+ mu-]cc): ' , lineD02MuMu.outputSelection()
  print 'Output location ([D* -> (D0 -> mu+ mu-) pi+]cc): ' , lineDstarWithD02MuMu.outputLocation()
  print 'Output selection ([D* -> (D0 -> mu+ mu-) pi+]cc): ' , lineDstarWithD02MuMu.outputSelection()
  print 'Output location ([D0 -> h h]cc): ' , lineD02HHForD02MuMu.outputLocation()
  print 'Output selection [(D0 -> h h]cc): ' , lineD02HHForD02MuMu.outputSelection()
  print 'Output location ([D* -> (D0 -> h h) pi+]cc): ' , lineDstarWithD02HHForD02MuMu.outputLocation()
  print 'Output selection ([D* -> (D0 -> h h) pi+]cc): ' , lineDstarWithD02HHForD02MuMu.outputSelection()
