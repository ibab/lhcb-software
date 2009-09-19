##############################################################################
# File for running Boole with Upgrade configuration, with full Digi output
#
# Syntax is:
#   gaudirun.py Boole-Upgrade-NoTruth.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGROOT/options/Boole/Upgrade-NoTruth.py")

##############################################################################
# I/O datasets and database tags are defined in a separate file,
##############################################################################
