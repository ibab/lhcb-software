#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__date__ = '22/05/2010'
__version__ = '$Revision: 1.5 $'

"""

This stripping line selects prompt D* in decay modes
[D*+ -> (D0 -> pi+ pi-, K+ K-, K- pi+, K+ pi-) pi+]cc.

Cuts:
  * Particles input: StdNoPIDsPions, StdNoPIDsKaons
  * IP-chi2 of the D0 daughters > 9
  * D0 mass window 100 MeV
  * chi2 of the D0 vertex < 16
  * chi2 of the D0 life time fit < 100 (loose cut for study non-propmpt D*)
  * D0 life time > 0.2 ps
  * M(D*) - M(D0) < 160 MeV
  * chi2 of the D* vertex < 100

"""

from PhysSelPython.Wrappers import DataOnDemand
pions = DataOnDemand(Location = 'Phys/StdNoPIDsPions')
kaons = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

# D0 combine
from Configurables import CombineParticles
D0Combine = CombineParticles( "CombD02HHForDstarPromptNoPt" )
D0Combine.DecayDescriptors = [ "[D0 -> pi+ pi-]cc", "[D0 -> K- pi+]cc", "[D0 -> K+ pi-]cc", "[D0 -> K+ K-]cc" ]
D0Combine.DaughtersCuts = {
  "pi+" : "BPVIPCHI2() > 9",
  "K+" : "BPVIPCHI2() > 9"
}
D0Combine.CombinationCut = "(ADAMASS('D0') < 110 * MeV)"
D0Combine.MotherCut = "(VFASPF(VCHI2) < 16) & (DMASS('D0') < 100 * MeV) & (BPVLTFITCHI2() < 100) & (BPVLTIME() > 0.2 * ps)"

# D* combine
DstarCombine = CombineParticles( "CombDstarPromptWithD02HHNoPt" )
DstarCombine.DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
DstarCombine.CombinationCut = "AM - AM1 < 170 * MeV"
DstarCombine.MotherCut = "(VFASPF(VCHI2) < 100) & ((M - M1) < 160 * MeV)"

# D0 and D* selections
from PhysSelPython.Wrappers import Selection
D0Selection = Selection("SelD02HHForDstarPromptNoPt", Algorithm = D0Combine, RequiredSelections = [ pions, kaons ])
DstarSelection = Selection("SelDstarPromptWithD02HHNoPt", Algorithm = DstarCombine, RequiredSelections = [ pions, kaons, D0Selection ])

# D* sequence
from PhysSelPython.Wrappers import SelectionSequence
DstarSequence = SelectionSequence("SeqDstarPromptWithD02HHNoPt", TopSelection = DstarSelection)

# D* stripping line
from StrippingConf.StrippingLine import StrippingLine
lineDstarPromptWithD02HHNoPt = StrippingLine('DstarPromptWithD02HHNoPt', prescale = 1, algos = [ DstarSequence ] )



if '__main__' == __name__ :

  print __doc__
  print 'Output location: ' , lineDstarPromptWithD02HHNoPt.outputLocation()
  print 'Output selection: ' , lineDstarPromptWithD02HHNoPt.outputSelection()
