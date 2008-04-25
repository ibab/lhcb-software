##############################################################################
# File for producing .rdst in DC06 workflows, reconstructing only L0 yes events
# Equivalent to DC06.opts with Init.DetectorList += { "L0" } enabled
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().withMC     = False
Brunel().recL0Only  = True
Brunel().outputType = "RDST"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
