#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-05-14
#

from Gaudi.Configuration import *
from Configurables import GaudiSequencer
from StrippingConf.StrippingLine import StrippingLine, strippingLines
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

StrippingConf().ActiveLines = []
                                                      # Specify the active stripping lines
					              # If empty (default), all defined StrippingLines will be active
StrippingConf().OutputType = "ETC"                    # Can be either "ETC" or "DST"

StrippingConf().MainOptions = "$STRIPPINGSELECTIONSROOT/options/StrippingSelections.py"

#
# Configure DaVinci
#

from Configurables import DaVinci

DaVinci().EvtMax = 100                         # Number of events
DaVinci().DataType = "MC09"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().ETCFile = "etc.root"

importOptions("$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.py")
