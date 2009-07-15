##############################################################################
# File for running Boole with MC09 configuration, with Minimal Digi output
#
# Syntax is:
#   gaudirun.py Boole-MC09-NoTruth.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGROOT/options/Boole/MC09-NoTruth.py")

from Configurables import Boole
Boole().DataType  = "MC09"

##############################################################################
# I/O datasets and database tags are defined in a separate file,
# see examples in MC09-Files.py
##############################################################################
