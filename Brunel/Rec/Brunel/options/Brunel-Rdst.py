##############################################################################
# File for producing .rdst with default settings
# (real data, field on, default database tags)
# Syntax is:
#   gaudirun.py Brunel-Rdst.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel

Brunel().OutputType = "RDST"

from GaudiKernel.Constants import *
Brunel().OutputLevel = WARNING

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
