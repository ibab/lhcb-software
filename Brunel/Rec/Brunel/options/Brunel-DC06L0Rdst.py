##############################################################################
# File for producing .rdst in DC06 workflows, reconstructing only L0 yes events
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DataType   = "DC06"
Brunel().InputType  = "DIGI"
Brunel().OutputType = "RDST"
Brunel().RecL0Only  = True
Brunel().NoWarnings = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
