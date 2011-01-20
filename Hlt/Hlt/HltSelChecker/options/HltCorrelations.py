from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import HltCorrelations, ReadHltReport
DaVinci().appendToMainSequence( [ HltCorrelations(), ReadHltReport() ]  )

DaVinci().EvtMax = 1000
DaVinci().DataType = "2010"
