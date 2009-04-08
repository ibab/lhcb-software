##############################################################################
# File for running Boole with MC09 configuration, with full Digi output
#
# Syntax is:
#   gaudirun.py Boole-MC09-WithTruth.py <someDataFiles>.py
##############################################################################

from Configurables import Boole

# just instantiate the configurable
theApp = Boole()

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in MC09-Files.py
##############################################################################
