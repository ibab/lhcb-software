#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__email__ = 'akozlins@cern.ch'
__date__ = '22/05/2010'
__version__ = '$Revision: 1.8 $'

"""

-- VERSION FOR RESTRIPPING OF THE FIRST 16nb --

This stripping line selects prompt D* in decay modes
[D*+ -> (D0 -> pi+ pi-, K+ K-, K- pi+, K+ pi-) pi+]cc.

Cuts:
  * Particles input: StdNoPIDsPions, StdNoPIDsKaons
  * Track-Chi2/NDOF of the D0 daughters < 10
  * Kaon from D0: PIDK - PIDpi > 0; pion from D0: PIDpi - PIDK > 0
  * IP-Chi2 of the D0 daughters > 6
  * D0 mass window 75 MeV
  * Chi2 of the D0 vertex < 25
  * Chi2 of the D0 life time fit < 100
  * D0 life time > 0.2 ps
  * Track-Chi2/NDOF of the soft pion from D* < 10
  * M(D*) - M(D0) < 155 MeV
  * Chi2 of the D* vertex < 100
  * 1 <= N(PV) <= 3

Note: designed for cross-sections measurement.

Rates on Reco05-Stripping08_SDSTs:
-------------------------------------------------
Decision name                     : Rate   Mult.
-------------------------------------------------
StrippingDstarPromptWithD02HHNoPt :  6.0%   2.0
StrippingD02HHPromptNoPt          : 17.6%   3.5
-------------------------------------------------

CPU usage on Reco05-Stripping08_SDSTs:
  StrippingDstarPromptWithD02HHNoPt : 2.3 ms
  StrippingD02HHPromptNoPt          : 1.6 ms

"""

D0DaughtersCutsPion = \
  "(TRCHI2DOF < 10) & (PIDpi - PIDK > 0) & (BPVIPCHI2() > 6)"
D0DaughtersCutsKaon = \
  "(TRCHI2DOF < 10) & (PIDK - PIDpi > 0) & (BPVIPCHI2() > 6)"
D0CombinationCut = \
  "(ADAMASS('D0') < 80 * MeV)"
D0MotherCut = \
  "(VFASPF(VCHI2) < 25) & " + \
  "(ADMASS('D0') < 75 * MeV) & " + \
  "(BPVLTFITCHI2() < 100) & " + \
  "(BPVLTIME() > 0.2 * ps)"

DstarDaughtersCutsPion = \
  "(TRCHI2DOF < 10)"
DstarCombinationCut = \
  "AM - AM1 < 160 * MeV"
DstarMotherCut = \
  "(VFASPF(VCHI2) < 100) & " + \
  "((M - M1) < 155 * MeV)"

from PhysSelPython.Wrappers import DataOnDemand
pions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
kaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

# D0 combine
from Configurables import CombineParticles
D0Combine = CombineParticles( "CombD02HHForDstarPromptNoPt" )
D0Combine.DecayDescriptors = [ "[D0 -> pi+ pi-]cc", "[D0 -> K- pi+]cc", "[D0 -> K+ pi-]cc", "[D0 -> K+ K-]cc" ]
D0Combine.DaughtersCuts = { "pi+" : D0DaughtersCutsPion, "K+" : D0DaughtersCutsKaon }
D0Combine.CombinationCut = D0CombinationCut
D0Combine.MotherCut = D0MotherCut

# D* combine
DstarCombine = CombineParticles( "CombDstarPromptWithD02HHNoPt" )
DstarCombine.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
DstarCombine.DaughtersCuts = { "pi+" : DstarDaughtersCutsPion }
DstarCombine.CombinationCut = DstarCombinationCut
DstarCombine.MotherCut = DstarMotherCut

# D0 and D* selections
from PhysSelPython.Wrappers import Selection
D0Selection = Selection("SelD02HHForDstarPromptNoPt", Algorithm = D0Combine, RequiredSelections = [ pions, kaons ])
DstarSelection = Selection("SelDstarPromptWithD02HHNoPt", Algorithm = DstarCombine, RequiredSelections = [ pions, kaons, D0Selection ])

# D* stripping line
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__VoidFilter
lineD02HHPromptNoPt = StrippingLine("D02HHPromptNoPt",
                                    checkPV = (1, 3),
                                    algos = [ D0Selection ],
                                    prescale = 1)
lineDstarPromptWithD02HHNoPt = StrippingLine("DstarPromptWithD02HHNoPt",
                                             checkPV = (1, 3),
                                             algos = [ DstarSelection ],
                                             prescale = 1)

if '__main__' == __name__ :

  print __doc__
  print 'Output location (D0): ' , lineD02HHPromptNoPt.outputLocation()
  print 'Output selection (D0): ' , lineD02HHPromptNoPt.outputSelection()
  print 'Output location (D*): ' , lineDstarPromptWithD02HHNoPt.outputLocation()
  print 'Output selection (D*): ' , lineDstarPromptWithD02HHNoPt.outputSelection()
