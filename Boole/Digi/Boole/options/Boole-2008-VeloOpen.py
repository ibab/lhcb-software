##############################################################################
# File for running Boole with default 2008 geometry, as defined in
#                        DDDB/Configuration.py but with open Velo
# Syntax is:
#   gaudirun.py Boole-2008-VeloOpen.py <someDataFiles>.py
##############################################################################

from Configurables import CondDB
CondDB().LocalTags["SIMCOND"] = ["velo-open"]

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
