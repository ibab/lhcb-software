##############################################################################
# File for Boole production of MDF files for 2009 Full Experiment System Test
#
# Syntax is:
#   gaudirun.py Boole-FEST09.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGROOT/options/Boole/FEST-L0Yes.py")

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in MC09-Files.py
##############################################################################
