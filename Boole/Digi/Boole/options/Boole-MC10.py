##############################################################################
# File for running Boole with default MC10 settings (full Digi output)
#
# Syntax is:
# gaudirun.py Boole-MC10.py <someDataFiles>.py
##############################################################################

from Gaudi.Configuration import importOptions
from Boole.Configuration import *
Boole().SiG4EnergyDeposit=False

importOptions("$APPCONFIGOPTS/Boole/DataType-2010.py")
 
