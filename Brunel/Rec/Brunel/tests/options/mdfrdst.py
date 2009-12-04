# Options to set up MDF RDST configuration test for Brunel
from Brunel.Configuration import *

Brunel().OutputType = 'RDST'
Brunel().PackType   = 'MDF'

Brunel().Monitors  = ["FPE","SC"]
