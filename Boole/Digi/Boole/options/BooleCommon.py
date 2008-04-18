# File to setup standard sequence for the Boole application.

# Syntax is:
#   gaudirun.py ../options/BooleCommon.py ../options/<specificOptions>.py
#
from Gaudi.Configuration import *

import GaudiKernel.ProcessJobOptions
GaudiKernel.ProcessJobOptions.printing_level += 1
importOptions('$BOOLEOPTS/Boole.opts')
