##############################################################################
# File for running Boole with MC09 configuration, with full Digi output
#
# Syntax is:
#   gaudirun.py Boole-MC09-WithTruth.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGROOT/options/Boole/MC09-WithTruth.py")

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in MC09-Files.py
##############################################################################
