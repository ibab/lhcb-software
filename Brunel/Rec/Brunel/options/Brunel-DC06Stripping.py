##############################################################################
# File for running Brunel in DC06 stripping workflow, starting from strip ETC
# Syntax is:
#   gaudirun.py Brunel-DC06Stripping.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().DDDBtag    = "DC06-default"
Brunel().condDBtag  = "DC06-default"
Brunel().inputType  = "ETC"
Brunel().withMC     = True
Brunel().noWarnings = True

Brunel().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in DC06-Files.py
##############################################################################
