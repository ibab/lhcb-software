##############################################################################
# File for running Brunel on real data in MC09 stripping workflow (No MC truth)
# starting from strip ETC
# Syntax is:
#   gaudirun.py Brunel-MC09Stripping.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel
from GaudiKernel.Constants import *

Brunel().InputType   = "ETC"
Brunel().Simulation  = "True"
Brunel().DataType    = "MC09"
Brunel().OutputLevel = WARNING

##############################################################################
# I/O datasets are defined in a separate file, see examples in MC09-Files.py
##############################################################################
