##############################################################################
# File for running Boole with default DC06 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Boole-DC06.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().UseSpillover = True
Boole().DataType     = "DC06"

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
