##############################################################################
# File for running Boole with latest supported 2008 gemoetry
# Syntax is:
#   gaudirun.py Boole-2008.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().DDDBtag     = "head-20080422"
Boole().condDBtag   = "head-20080422"

Boole().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
