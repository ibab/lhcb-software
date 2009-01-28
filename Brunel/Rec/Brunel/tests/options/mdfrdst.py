# Options to set up MDF RDST configuration test for Brunel
from Brunel.Configuration import *

Brunel().OutputType = 'RDST'
Brunel().PackType   = 'MDF'

Brunel().EvtMax = 0
Brunel().DDDBtag   = "default"
Brunel().CondDBtag = "default"
Brunel().Monitors  = ["FPE","SC"]
