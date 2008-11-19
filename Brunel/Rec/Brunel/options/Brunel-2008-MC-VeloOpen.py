##############################################################################
# File for running Brunel on MC data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-2008-MC.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().InputType    = "DIGI"
Brunel().WithMC       = True
Brunel().UseSimCond   = True
Brunel().SpecialData  = ["veloOpen"]

#--Specify SIMCOND tag for Open VELO
from Configurables import CondDB
CondDB().LocalTags["SIMCOND"] = ["velo-open"]

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
