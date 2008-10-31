##############################################################################
# File for running Boole with default 2008 geometry, as defined in
#                $DDDBROOT/options/lhcb-2008.py but with open Velo
# Syntax is:
#   gaudirun.py Boole-2008-VeloOpen.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().useSpillover = False
Boole().DDDBtag      = "2008-default"
Boole().condDBtag    = "2008-default"

#--Tell SIMCOND tag to generate Open VELO
from DetCond.Configuration import addCondDBLayer
simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
simCondVelo.DefaultTAG = "velo-open"
addCondDBLayer(simCondVelo)

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
