##############################################################################
# File for running Boole with default MC10 settings (full Digi output)
#
# Syntax is:
# gaudirun.py Boole-MC10.py <someDataFiles>.py
##############################################################################

from Gaudi.Configuration import importOptions

importOptions("$APPCONFIGOPTS/Boole/Default.py")
importOptions("$APPCONFIGOPTS/Boole/DataType-2010.py")
importOptions("$APPCONFIGOPTS/Boole/IgnoreFlatSpillover.py")
 
