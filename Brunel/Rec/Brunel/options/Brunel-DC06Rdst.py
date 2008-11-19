##############################################################################
# File for producing .rdst in DC06 production workflows.
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DataType   = "DC06"
Brunel().InputType  = "DIGI"
Brunel().OutputType = "RDST"
Brunel().NoWarnings = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
