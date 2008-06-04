##############################################################################
# File for running Boole with default 2008 geometry, as defined in
#                                              $DDDBROOT/options/lhcb-2008.py
# Syntax is:
#   gaudirun.py Boole-2008.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().DDDBtag     = "2008-default"
Boole().condDBtag   = "lhcb-2008"

Boole().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
