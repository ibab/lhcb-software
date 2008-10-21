##############################################################################
# File for running Brunel on MC data with default 2008 geometry, as defined in
#                                              $DDDBROOT/options/lhcb-2008.py
# Syntax is:
#   gaudirun.py Brunel-2008-MC.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().inputType    = "DIGI"
Brunel().withMC       = True
Brunel().useSimCond   = True
Brunel().specialData  = ["fieldOff","veloOpen"]
Brunel().applyConf()

#--Specify SIMCOND tag for Open VELO
from DetCond.Configuration import addCondDBLayer
simCondVelo = allConfigurables["SIMCOND"].clone("VELOCOND")
simCondVelo.DefaultTAG = "velo-open"
addCondDBLayer(simCondVelo)

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
