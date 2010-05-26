
__author__ = 'Alexandr Kozlinskiy'
__date__ = '22/05/2010'
__version__ = '$Revision: 1.2 $'

from PhysSelPython.Wrappers import DataOnDemand
pions = DataOnDemand(Location = 'Phys/StdNoPIDsPions')
kaons = DataOnDemand(Location = 'Phys/StdNoPIDsKaons')

from Configurables import CombineParticles
D0Combine = CombineParticles( "D02PiPiKPiKK" )
D0Combine.DecayDescriptors = [ "[D0 -> pi+ pi-]cc", "[D0 -> K- pi+]cc", "[D0 -> K+ K-]cc" ]
D0Combine.DaughtersCuts = {
  "pi+" : "BPVIPCHI2() > 6.25",
  "K+" : "BPVIPCHI2() > 6.25"
}
D0Combine.CombinationCut = "(ADAMASS('D0') < 100 * MeV)"
D0Combine.MotherCut = "(VFASPF(VCHI2/VDOF) < 9) & (BPVLTFITCHI2() < 9) & (BPVLTIME() > 0.3 * ps)"

DstarCombine = CombineParticles( "Dstar2D0PiWithD02PiPiKPiKK" )
DstarCombine.DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
DstarCombine.DaughtersCuts = {
  'pi+' : "BPVIPCHI2() < 100"
}
DstarCombine.CombinationCut = "(ADAMASS('D*(2010)+') < 500 * MeV)"
DstarCombine.MotherCut = "(VFASPF(VCHI2/VDOF) < 16) & ((M - MAXTREE('D0' == ABSID, M)) < 160 * MeV)"

from PhysSelPython.Wrappers import Selection
D0Selection = Selection("SelectionD02PiPiKPiKK", Algorithm = D0Combine, RequiredSelections = [ pions, kaons ])
DstarSelection = Selection("SelectionDstar2D0PiWithD02PiPiKPiKK", Algorithm = DstarCombine, RequiredSelections = [ pions, kaons, D0Selection ])

from PhysSelPython.Wrappers import SelectionSequence
DstarSequence = SelectionSequence("SequenceDstarPromptWithD02HHNoPt", TopSelection = DstarSelection)

from StrippingConf.StrippingLine import StrippingLine
lineDstarPromptWithD02HHNoPt = StrippingLine('DstarPromptWithD02HHNoPt', prescale = 1, algos = [ DstarSequence ] )
