##############################################################################
# File for running Brunel on MC data with default MC09 settings,
# reading and saving no MC Truth, except for pileup information
#
# Syntax is:
#   gaudirun.py Brunel-MC09-NoTruth.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().InputType = "DIGI" # implies also Brunel().Simulation = True
Brunel().DigiType  = "Minimal"
Brunel().DataType  = "MC09"

##############################################################################
# I/O datasets are defined in a separate file, see examples in MC09-Files.py
##############################################################################
