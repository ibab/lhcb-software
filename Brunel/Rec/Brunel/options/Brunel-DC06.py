##############################################################################
# File for running Brunel on DC06 data with default DC06 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Brunel-DC06.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DataType     = "DC06"
Brunel().InputType    = "DIGI"
Brunel().WithMC       = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
