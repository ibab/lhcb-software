##############################################################################
# File for running Brunel in DC06 stripping workflow, starting from strip ETC
# Syntax is:
#   gaudirun.py Brunel-DC06Stripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DataType   = "DC06"
Brunel().InputType  = "ETC"
Brunel().WithMC     = True
Brunel().NoWarnings = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
