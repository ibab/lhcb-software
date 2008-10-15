##############################################################################
# File for running Brunel on real data in default stripping workflow,
# (real data, field on, default database tags) starting from strip ETC
# Syntax is:
#   gaudirun.py Brunel-Stripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().inputType  = "ETC"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
