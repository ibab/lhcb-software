##############################################################################
# File for running Boole with default DC06 geometry, as defined in
#                                                        DDDB/Configuration.py
# Syntax is:
#   gaudirun.py Boole-DC06.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().UseSpillover = True
Boole().DDDBtag      = "DC06-default"
Boole().CondDBtag    = "DC06-default"

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
