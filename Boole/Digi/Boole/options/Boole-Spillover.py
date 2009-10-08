##############################################################################
# File for running spillover in Boole
#
# Syntax is:
#   gaudirun.py Boole-Spillover.py <someDataFiles>.py
##############################################################################

from Configurables import Boole
Boole().UseSpillover = True

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in Spillover-Files.py
##############################################################################
