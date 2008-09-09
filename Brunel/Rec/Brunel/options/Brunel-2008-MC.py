##############################################################################
# File for running Brunel on MC data with default 2008 geometry, as defined in
#                                              $DDDBROOT/options/lhcb-2008.py
# Syntax is:
#   gaudirun.py Brunel-2008-MC.py <someDataFiles>.py
##############################################################################

from Brunel.Configuration import *

Brunel().inputType    = "DIGI"
Brunel().withMC       = True

Brunel().applyConf()


##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
