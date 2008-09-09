##############################################################################
# File for producing .rdst from 2008 real data.
# Syntax is:
#   gaudirun.py Brunel-2008-Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().outputType = "RDST"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
