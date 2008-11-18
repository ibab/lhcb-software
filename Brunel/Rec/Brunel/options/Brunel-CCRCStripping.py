##############################################################################
# File for running Brunel in CCRC stripping workflow, starting from an ETC
# referring to a raw data file without MC Truth (e.g. .MDF)
# Equivalent to RealData-ETC.opts
#
# Syntax is:
#   gaudirun.py Brunel-CCRCStripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().InputType  = "ETC"
Brunel().NoWarnings = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
