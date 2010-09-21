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
  * Track-Chi2/NDOF of the D0 daughters < 10
  * Pt of the D0 daughters > 1 GeV
  * D0 mass window 100 MeV
  * Chi2 of the D0 vertex < 100
  * Chi2 of the D0 life time fit < 100
  * D0 life time > 0.1 ps
  * Track-Chi2/NDOF of the soft pion from D* < 10
  * M(D*) - M(D0) < 155 MeV
  * Chi2 of the D* vertex < 100
  * HLT = 'Hlt.*(MuMu|MBMicro|Muon).*Decision'

Rates on Reco05-Stripping08_SDSTs:
--------------------------------------------
Decision name                     : Rate
--------------------------------------------
StrippingD02MuMu                  :   0.01%
StrippingDstarWithD02MuMu         : < 0.01%
StrippingD02HHForD02MuMu          :   0.13%    (prescale = 0.02)
StrippingDstarWithD02HHForD02MuMu : < 0.03%    (prescale = 0.02)
--------------------------------------------

CPU usage on Reco05-Stripping08_SDSTs:
  StrippingD02MuMu: 0.7 ms.
  StrippingDstarWithD02MuMu: 0.1 ms.
  StrippingD02HHForD02MuMu: 2.2 ms.
  StrippingDstarWithD02HHForD02MuMu: 1.6 ms.

"""

D0DaughtersCutsMuons = \
  "(TRCHI2DOF < 10) & (PT > 1 * GeV)"
D0DaughtersCutsPion = \
  "(TRCHI2DOF < 10) & (PT > 1 * GeV)"
D0DaughtersCutsKaon = \
  "(TRCHI2DOF < 10) & (PT > 1 * GeV)"
D0CombinationCut = \
  "(ADAMASS('D0') < 110 * MeV)"
D0MotherCut = \
  "(VFASPF(VCHI2) < 100) & " + \
  "(ADMASS('D0') < 100 * MeV) & " + \
  "(BPVLTFITCHI2() < 100) & " + \
  "(BPVLTIME() > 0.1 * ps)"

DstarDaughtersCutsPion = \
  "(TRCHI2DOF < 10)"
DstarCombinationCut = \
  "AM - AM1 < 160 * MeV"
DstarMotherCut = \
  "(VFASPF(VCHI2) < 100) & " + \
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
  prescale = 0.02,
  HLT = "(HLT_PASS_RE('Hlt.*(MuMu|MBMicro|Muon).*Decision'))")

lineDstarWithD02HHForD02MuMu = StrippingLine("DstarWithD02HHForD02MuMu",
  algos = [ DstarWithD02HHSelection ],
  prescale = 0.02,
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
