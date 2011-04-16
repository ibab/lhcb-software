
from Brunel.Configuration import *

importOptions("CommonOptions.py")

importOptions("/usera/jonesc/MyDataOptions/Collisions10/Raw/RUN_76997_RealData_90000000_RAW_PFNs.py")

from Configurables import Brunel

Brunel().EvtMax    = 10
Brunel().PrintFreq = 1
