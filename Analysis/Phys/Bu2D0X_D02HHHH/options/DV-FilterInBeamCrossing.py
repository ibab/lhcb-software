from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import LoKi__ODINFilter  as ODINFilter
odinFilter=ODINFilter( 'FilterInBeamCrossing'
                       , Code = " ODIN_BXTYP == LHCb.ODIN.BeamCrossing " )
#DaVinci().appendToMainSequence(odinFilter)
useralgs=DaVinci().UserAlgorithms
DaVinci().UserAlgorithms=[odinFilter]+useralgs
