#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__email__ = 'akozlins@cern.ch'
__date__ = '31/08/2010'
__version__ = '$Revision: 1.1 $'

"""

This stripping line selects prompt D* and D0 in decay modes
[D*+ -> (D0 -> mu+ mu-, pi+ pi-, K+ K-, K- pi+, K+ pi-) pi+]cc
with HLT lines 'Hlt2Charm.*D02MuMuDecision|Hlt1MBMicro.*Decision|Hlt2.*MuonDecision'

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

Rates on Reco05-Stripping08_SDSTs:
-------------------------------------------------
Decision name                     : Rate   Mult.
-------------------------------------------------
StrippingD02MuMu                  : 0.0%   1.0
StrippingDstarWithD02MuMu         : 0.0%   ---
StrippingD02HHForD02MuMu          : 0.3%   2.5    (prescaled by 0.1)
StrippingDstarWithD02HHForD02MuMu : 0.1%   2.6    (prescaled by 0.1)
-------------------------------------------------

CPU usage on Reco05-Stripping08_SDSTs:
  StrippingD02MuMu: 1 ms.
  StrippingDstarWithD02MuMu: 0 ms.
  StrippingD02HHForD02MuMu: 0 ms.
  StrippingDstarWithD02HHForD02MuMu: 0 ms.

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
  HLT = "(HLT_PASS_RE('Hlt2Charm.*D02MuMuDecision|Hlt1MBMicro.*Decision|Hlt2.*MuonDecision'))")
lineDstarWithD02MuMu = StrippingLine("DstarWithD02MuMu",
  algos = [ DstarWithD02MuMuSelection ],
  prescale = 1,
  HLT = "(HLT_PASS_RE('Hlt2Charm.*D02MuMuDecision|Hlt1MBMicro.*Decision|Hlt2.*MuonDecision'))")
lineD02HHForD02MuMu = StrippingLine("D02HHForD02MuMu",
  algos = [ D02HHSelection ],
  prescale = 0.05,
  HLT = "(HLT_PASS_RE('Hlt2Charm.*D02MuMuDecision|Hlt1MBMicro.*Decision|Hlt2.*MuonDecision'))")
lineDstarWithD02HHForD02MuMu = StrippingLine("DstarWithD02HHForD02MuMu",
  algos = [ DstarWithD02HHSelection ],
  prescale = 0.05,
  HLT = "(HLT_PASS_RE('Hlt2Charm.*D02MuMuDecision|Hlt1MBMicro.*Decision|Hlt2.*MuonDecision'))")

if '__main__' == __name__ :

  print __doc__
  print 'Output location (D0 -> mu+ mu-): ' , lineD02MuMu.outputLocation()
  print 'Output selection (D0 -> mu+ mu-): ' , lineD02MuMu.outputSelection()
  print 'Output location (D*): ' , lineDstarWithD02MuMu.outputLocation()
  print 'Output selection (D*): ' , lineDstarWithD02MuMu.outputSelection()
