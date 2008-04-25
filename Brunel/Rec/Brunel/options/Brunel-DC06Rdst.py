##############################################################################
# File for producing .rdst in DC06 production workflows.
# Equivalent to DC06.opts
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().withMC     = False
Brunel().outputType = "RDST"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
