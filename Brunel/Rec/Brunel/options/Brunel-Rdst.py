##############################################################################
# File for producing .rdst with default settings
# (real data, field on, default database tags)
# Syntax is:
#   gaudirun.py Brunel-Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().outputType = "RDST"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
