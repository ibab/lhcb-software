##############################################################################
# File for running Boole with latest supported 2008 gemoetry
# Syntax is:
#   gaudirun.py Boole-2008.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().DDDBtag     = "lhcb-2008"
Boole().condDBtag   = "lhcb-2008"

Boole().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
