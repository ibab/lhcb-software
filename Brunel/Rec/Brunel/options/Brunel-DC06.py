##############################################################################
# File for running Brunel on DC06 data with default DC06 geometry, as defined in
#                                              $DDDBROOT/options/DC06.py
# Syntax is:
#   gaudirun.py Brunel-DC06.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DDDBtag      = "DC06-default"
Brunel().condDBtag    = "DC06-default"
Brunel().inputType    = "DIGI"
Brunel().withMC       = True

Brunel().applyConf()


##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
