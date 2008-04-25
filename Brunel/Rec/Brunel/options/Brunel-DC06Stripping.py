##############################################################################
# File for running Brunel in DC06 stripping workflow, starting from strip ETC
# Equivalent to DC06-ETC.opts
#
# Syntax is:
#   gaudirun.py Brunel-DC06Stripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().inputType  = "ETC"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
