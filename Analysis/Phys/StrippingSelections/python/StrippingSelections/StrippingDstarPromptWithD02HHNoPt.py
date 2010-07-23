#!/usr/bin/env python

__author__ = 'Alexandr Kozlinskiy'
__email__ = 'akozlins@cern.ch'
__date__ = '22/05/2010'
__version__ = '$Revision: 1.6 $'

"""

This stripping line selects prompt D* in decay modes
[D*+ -> (D0 -> pi+ pi-, K+ K-, K- pi+, K+ pi-) pi+]cc.

Cuts:
  * Particles input: StdNoPIDsPions, StdNoPIDsKaons
  * Track-Chi2/NDOF of the D0 daughters < 10
  * IP-Chi2 of the D0 daughters > 9
  * D0 mass window 75 MeV
  * Chi2 of the D0 vertex < 9
  * Chi2 of the D0 life time fit < 36
  * D0 life time > 0.3 ps
  * Track-Chi2/NDOF of the soft pion from D* < 10
  * M(D*) - M(D0) < 155 MeV
  * Chi2 of the D* vertex < 100

"""

from PhysSelPython.Wrappers import DataOnDemand
pions = DataOnDemand(Location = "Phys/StdNoPIDsPions")
kaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

# D0 combine
from Configurables import CombineParticles
D0Combine = CombineParticles( "CombD02HHForDstarPromptNoPt" )
D0Combine.DecayDescriptors = [ "D0 -> pi+ pi-", "D0 -> K- pi+", "D0 -> K+ pi-", "D0 -> K+ K-" ]
D0Combine.DaughtersCuts = {
  "pi+" : "(TRCHI2DOF < 10) & (BPVIPCHI2() > 9)",
  "K+" : "(TRCHI2DOF < 10) & (BPVIPCHI2() > 9)"
}
D0Combine.CombinationCut = "(ADAMASS('D0') < 80 * MeV)"
D0Combine.MotherCut = "(VFASPF(VCHI2) < 9) & (DMASS('D0') < 75 * MeV) & (BPVLTFITCHI2() < 36) & (BPVLTIME() > 0.3 * ps)"

# D* combine
DstarCombine = CombineParticles( "CombDstarPromptWithD02HHNoPt" )
DstarCombine.DecayDescriptors = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
DstarCombine.DaughtersCuts = {
  "pi+" : "(TRCHI2DOF < 10)"
}
DstarCombine.CombinationCut = "AM - AM1 < 160 * MeV"
DstarCombine.MotherCut = "(VFASPF(VCHI2) < 100) & ((M - M1) < 155 * MeV)"

# D0 and D* selections
from PhysSelPython.Wrappers import Selection
D0Selection = Selection("SelD02HHForDstarPromptNoPt", Algorithm = D0Combine, RequiredSelections = [ pions, kaons ])
DstarSelection = Selection("SelDstarPromptWithD02HHNoPt", Algorithm = DstarCombine, RequiredSelections = [ pions, kaons, D0Selection ])

# D* stripping line
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__VoidFilter
lineDstarPromptWithD02HHNoPt = StrippingLine("DstarPromptWithD02HHNoPt",
                                             algos = [
                                               LoKi__VoidFilter("NPVFilter", Code = "CONTAINS('Rec/Vertex/Primary') < 3.5"),
                                               DstarSelection ],
                                             prescale = 1)

if '__main__' == __name__ :

  print __doc__
  print 'Output location: ' , lineDstarPromptWithD02HHNoPt.outputLocation()
  print 'Output selection: ' , lineDstarPromptWithD02HHNoPt.outputSelection()
