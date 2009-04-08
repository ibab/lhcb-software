##############################################################################
# File for running Boole with MC09 configuration, with Minimal Digi output
#
# Syntax is:
#   gaudirun.py Boole-MC09-NoTruth.py <someDataFiles>.py
##############################################################################

from Configurables import Boole

# Switch off MC truth output except for primary vertices information
Boole().DigiType = 'Minimal'

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in MC09-Files.py
##############################################################################
