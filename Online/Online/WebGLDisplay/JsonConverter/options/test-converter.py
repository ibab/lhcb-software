# Config for performance testing 
from Gaudi.Configuration import *

importOptions("$BRUNELROOT/options/Brunel-Default.py")
importOptions("$PRCONFIGOPTS/Brunel/PR-COLLISION12-Beam4000GeV-VeloClosed-MagDown.py")

from Configurables import Brunel
Brunel().EvtMax=10

from Configurables import JsonConverter
conv = JsonConverter()

GaudiSequencer("OutputDSTSeq").Members += [ conv ]

