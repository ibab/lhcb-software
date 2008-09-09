##############################################################################
# File for running Brunel on real data in 2008 stripping workflow,
# starting from strip ETC
# Syntax is:
#   gaudirun.py Brunel-2008-Stripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().inputType  = "ETC"
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
