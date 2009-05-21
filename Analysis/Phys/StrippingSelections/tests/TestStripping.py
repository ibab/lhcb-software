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

StrippingConf().ActiveLines = []                      # Specify the active stripping lines
					              # If empty (default), all defined StrippingLines will be active

StrippingConf().ActiveStreams = []                    # Specify the active streams. Only selections from 
                                                      # these streams will run. If empty, all streams are active

StrippingConf().OutputType = "DST"                    # Can be either "ETC" or "DST"

#
# Configure DaVinci
#

from Configurables import DaVinci, DaVinciWriteDst

DaVinci().EvtMax = 100                         # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().ETCFile = "etc.root"
DaVinci().Simulation   = True
DaVinci().HistogramFile = "TestStripping.root"

#
# Import input DST files (B->JpsiMuMu)
#
importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")
