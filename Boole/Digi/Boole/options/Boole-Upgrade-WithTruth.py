##############################################################################
# File for running Boole with Upgrade configuration, with full Digi output
#
# Syntax is:
#   gaudirun.py Boole-Upgrade-WithTruth.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGROOT/options/Boole/Upgrade-WithTruth.py")

##############################################################################
# I/O datasets and database tags are defined in a separate file,
##############################################################################
