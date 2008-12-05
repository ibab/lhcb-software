##############################################################################
# File for Boole production of MDF files for 2009 Full Experiment System Test
#
# Syntax is:
#   gaudirun.py Boole-FEST09.py <someDataFiles>.py
##############################################################################

from Configurables import Boole

Boole().Outputs    = ["MDF"]
Boole().NoWarnings = True

##############################################################################
# I/O datasets are defined in a separate file, see examples in 2008-Files.py
##############################################################################
