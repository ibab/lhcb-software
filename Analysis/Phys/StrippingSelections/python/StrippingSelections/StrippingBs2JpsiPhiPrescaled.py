# $Id: StrippingBs2JpsiPhiPrescaled.py,v 1.2 2009-10-30 11:08:30 poluekt Exp $

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.2 $'

__all__ = ('name', 'Phi', 'Jpsi', 'Bs', 'sequence')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

name = "Bs2JpsiPhiPrescaled"

_phiFilter = FilterDesktop("PhiFilterFor"+name)
_phiFilter.Code = "(PT>500*MeV)"

_stdPhi2KK = DataOnDemand("stdLoosePhi2KK", Location = "Phys/StdLoosePhi2KK")

Phi = Selection ("PhiFor"+name,
                 Algorithm = _phiFilter,
                 RequiredSelections = [_stdPhi2KK])

Jpsi = DataOnDemand("stdLooseJpsi2MuMu", Location = "Phys/StdLooseJpsi2MuMu")

_BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<14) & (VFASPF(VCHI2)<20)"
_Bs = CombineParticles( name,
                        DecayDescriptor = "[B_s0 -> J/psi(1S) phi(1020)]cc",
                        MotherCut = _BsCuts)

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])

sequence = SelectionSequence("Seq"+name, TopSelection = Bs)

from StrippingConf.StrippingLine import StrippingLine, StrippingMember

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine("Bs2JpsiPhiPrescaledLine"
                          , prescale = 1.
                          , postscale = 0.1
                          , algos = [ sequence ]
                          )
