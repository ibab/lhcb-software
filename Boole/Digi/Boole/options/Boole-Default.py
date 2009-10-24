##############################################################################
# File for running Boole with default options
##############################################################################
# Syntax is:
#   gaudirun.py Boole-Default.py <someDataFiles>.py
##############################################################################

from Configurables import Boole

###############################################################################
# Set here any steering options.
# Available steering options and defaults are documented in
# $BOOLEROOT/python/Boole/Configuration.py
###############################################################################

# Just instantiate the configurable...
theApp = Boole()
##############################################################################
# I/O datasets are defined in a separate file, see examples in MC09-Files.py
##############################################################################
