from Gaudi.Configuration import *
from Configurables import LoKi__HDRFilter as HDRFilter
from Configurables import LoKi__L0Filter as L0Filter
# the sideband line
stripFilter=HDRFilter(
  "StripB2DX_D2HHHHFilter"
  ,Code="HLT_PASS('StrippingB2DXChi2LooseWithD2hhhhLineDecision')"
  ,Location="/Event/Strip/Phys/DecReports")

from PhysSelPython.Wrappers import AutomaticData
B2DX_D2HHHHSel = AutomaticData(Location="/Event/Hadronic/Phys/B2DXChi2LooseWithD2hhhhLine")

__all__=('B2DX_D2HHHHSel',)  
