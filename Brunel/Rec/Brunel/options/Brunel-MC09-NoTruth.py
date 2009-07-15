##############################################################################
# File for running Brunel on MC data with default MC09 settings,
# reading and saving no MC Truth, except for pileup information
#
# Syntax is:
#   gaudirun.py Brunel-MC09-NoTruth.py <someDataFiles>.py
##############################################################################

from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/MC09-NoTruth.py")

from Configurables import Brunel
Brunel().DataType  = "MC09"

##############################################################################
# I/O datasets are defined in a separate file, see examples in MC09-Files.py
##############################################################################
