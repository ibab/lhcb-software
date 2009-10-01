from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bs2JpsiPhiPrescaled"

_phiFilter = FilterDesktop("PhiFilterFor"+name)
_phiFilter.Code = "(PT>500*MeV)"

_stdPhi2KK = DataOnDemand("stdLoosePhi2KK", Location = "StdLoosePhi2KK")

Phi = Selection ("PhiFor"+name,
                 Algorithm = _phiFilter,
                 RequiredSelections = [_stdPhi2KK])
Phi.__apply_configuration__()

Jpsi = DataOnDemand("stdLooseJpsi2MuMu", Location = "StdLooseJpsi2MuMu")

_BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<14) & (VFASPF(VCHI2)<20)"
_Bs = CombineParticles( name,
                        DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc",
                        MotherCut = _BsCuts)

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])
Bs.__apply_configuration__()

sequence = SelectionSequence("Seq"+name, TopSelection = Bs)
sequence.__apply_configuration__()

__all__ = (name, Phi, Jpsi, Bs, sequence)
