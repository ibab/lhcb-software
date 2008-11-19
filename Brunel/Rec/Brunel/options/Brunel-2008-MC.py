##############################################################################
# File for running Brunel on MC data with default 2008 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-2008-MC.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().InputType    = "DIGI" # implies also Brunel().Simulation = True
Brunel().WithMC       = True   # implies also Brunel().Simulation = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
