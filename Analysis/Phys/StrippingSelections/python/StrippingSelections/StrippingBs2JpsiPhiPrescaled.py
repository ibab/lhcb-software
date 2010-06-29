# $Id: StrippingBs2JpsiPhiPrescaled.py,v 1.5 2010-06-29 15:18:51 jpalac Exp $

__author__ = ['Greig Cowan','Juan Palacios']
__date__ = '01/10/2009'
__version__ = '$Revision: 1.5 $'

__all__ = ('name', 'Phi', 'Jpsi', 'Bs')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand

name = "Bs2JpsiPhiPrescaled"

_phiFilter = FilterDesktop("PhiFilterFor"+name)
_phiFilter.Code = "(PT>500*MeV)"

_stdPhi2KK = DataOnDemand(Location = "Phys/StdLoosePhi2KK")

Phi = Selection ("PhiFor"+name,
                 Algorithm = _phiFilter,
                 RequiredSelections = [_stdPhi2KK])

Jpsi = DataOnDemand(Location = "Phys/StdLooseJpsi2MuMu")

_BsCuts = "(ADMASS('B_s0')<300*MeV) & (BPVLTFITCHI2()<14) & (VFASPF(VCHI2)<20)"
_Bs = CombineParticles( name,
                        DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)",
                        MotherCut = _BsCuts)

Bs = Selection ( "Sel"+name,
                 Algorithm = _Bs,
                 RequiredSelections = [Phi, Jpsi])

from StrippingConf.StrippingLine import StrippingLine

############################################
# Create StrippingLine with this selection #
############################################
line = StrippingLine("Bs2JpsiPhiPrescaledLine"
                          , prescale = 1.
                          , postscale = 0.1
                          , algos = [ Bs ]
                          )
