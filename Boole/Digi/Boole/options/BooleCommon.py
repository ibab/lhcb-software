# File to setup standard sequence for the Boole application.

# Syntax is:
#   gaudirun.py ../options/BooleCommon.py ../options/<specificOptions>.py
#
from Gaudi.Configuration import *

ApplicationMgr( AppName    = 'Boole', AppVersion = 'v14r11' )

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1
importOptions('$BOOLEOPTS/Boole.opts')
