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

#StrippingConf().ActiveStreams = ["BExclusive"]        # Specify the active streams. Only selections from 
                                                      # these streams will run. If empty, all streams are active

StrippingConf().OutputType = "ETC"                    # Can be either "ETC" or "DST"

StrippingConf().MainOptions = "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingSelections.py"

#
# Configure DaVinci
#

from Configurables import DaVinci

DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().Simulation   = True                  # It is MC
DaVinci().ETCFile = "etc.root"

#
# Import input DST files (B->JpsiMuMu inclusive)
#
# importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")
# importOptions("$STRIPPINGSELECTIONSROOT/tests/Fest.py")
# importOptions("$STRIPPINGSELECTIONSROOT/tests/DC06-MinimumBiasDst.py")
# importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-MinimumBiasDst.py")

EventSelector().Input   = [
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbrdst&castorVersion=2&path=/castor/cern.ch/grid/lhcb/data/2009/RDST/00004636/0000/00004636_00000419_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbrdst&castorVersion=2&path=/castor/cern.ch/grid/lhcb/data/2009/RDST/00004636/0000/00004636_00000420_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='castor://castorlhcb.cern.ch:9002/?svcClass=lhcbrdst&castorVersion=2&path=/castor/cern.ch/grid/lhcb/data/2009/RDST/00004636/0000/00004636_00000421_1.rdst' TYP='POOL_ROOTTREE' OPT='READ'" ]
