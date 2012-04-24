
from Brunel.Configuration import *

#importOptions("$APPCONFIGOPTS/UseOracle.py")

importOptions("CommonOptions.py")

from Configurables import Brunel

Brunel().EvtMax    = -1
Brunel().PrintFreq = 100
