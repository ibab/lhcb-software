##############################################################################
# File for running Brunel in CCRC stripping workflow, starting from an ETC
# referring to a raw data file without MC Truth (e.g. .MDF)
# Equivalent to RealData-ETC.opts
#
# Syntax is:
#   gaudirun.py Brunel-CCRCStripping.py <someDataFiles>.py
##############################################################################

from Configurables import Brunel
Brunel().InputType  = "ETC"

from GaudiKernel.Constants import *
Brunel().OutputLevel = WARNING

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
