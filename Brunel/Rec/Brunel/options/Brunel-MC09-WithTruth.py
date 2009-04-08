##############################################################################
# File for running Brunel on MC data with default MC09 settings,
# and saving all MC Truth
#
# Syntax is:
#   gaudirun.py Brunel-MC09-WithTruth.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().InputType = "DIGI" # implies also Brunel().Simulation = True
Brunel().WithMC    = True   # implies also Brunel().Simulation = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in MC09-Files.py
##############################################################################
