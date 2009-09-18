##############################################################################
# File for producing .rdst in DC06 production workflows.
# Syntax is:
#   gaudirun.py Brunel-DC06Rdst.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().DataType   = "DC06"
Brunel().InputType  = "DIGI"
Brunel().OutputType = "RDST"

from GaudiKernel.Constants import *
Brunel().OutputLevel = WARNING

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
