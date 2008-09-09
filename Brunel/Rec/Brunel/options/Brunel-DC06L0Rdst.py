##############################################################################
# File for producing .rdst in DC06 workflows, reconstructing only L0 yes events
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DDDBtag    = "DC06-default"
Brunel().condDBtag  = "DC06-default"
Brunel().inputType  = "DIGI"
Brunel().outputType = "RDST"
Brunel().recL0Only  = True
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
