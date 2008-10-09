##############################################################################
# File for Boole production of MDF files for 2009 Full Scale Test
#
# Syntax is:
#   gaudirun.py Boole-FST09.py <someDataFiles>.py
##############################################################################

from Boole.Configuration import *

Boole().useSpillover = False
Boole().writeRawMDF  = True
Boole().writeDigi    = False
Boole().noWarnings   = True
Boole().DDDBtag      = "2008-default"
Boole().condDBtag    = "2008-default"

Boole().applyConf()

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
