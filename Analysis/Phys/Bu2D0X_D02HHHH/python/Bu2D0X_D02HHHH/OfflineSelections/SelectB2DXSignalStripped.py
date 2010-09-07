from Gaudi.Configuration import *
from Configurables import LoKi__HDRFilter as HDRFilter
from Configurables import LoKi__L0Filter as L0Filter
# the signal line
stripFilter=HDRFilter(
  "StrippedB2DX_D02HHHHSignalFilter"
  ,Code="HLT_PASS('B2DXChi2LooseWithD2hhhhSignalLineDecision')"
  ,Location="/Event/Strip/Phys/DecReports")

from PhysSelPython.Wrappers import AutomaticData
B2DX_D2HHHHSignalSel = AutomaticData(Location="/Event/Hadronic/Phys/B2DXChi2LooseWithD2hhhhSignalLine")

__all__=('B2DX_D2HHHHSignalSel',)  
