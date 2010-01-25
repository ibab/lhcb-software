#
#   Stripping selections job (DST output)
#
#   @author J. Palacios/A. Poluektov
#   @date 2009-11-05
#

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
##############################################################################
#
# Monitoring
#
importOptions( "$DAVINCIMONITORSROOT/options/DaVinciMonitoring.py") 
##############################################################################
#
# Stripping
#
from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Streams import allStreams
from StrippingSelections import StreamMiniBias
noMB = allStreams
noMB.remove( StreamMiniBias.stream )

sc = StrippingConf( Streams = noMB )
# Dirac should modify OutputFilePrefix.
# SelDSTWriter("StripMC09DSTWriter").OutputFileSuffix = '012345'
dstWriter = SelDSTWriter("StripMC09DSTWriter",
                         SelectionSequences = sc.activeStreams(),
                         OutputPrefix = 'Strip',
                         ExtraItems = ['/Event/DAQ/RawEvent']
                         )
##############################################################################
#
# Hlt
#
DaVinci().Hlt = True
DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Aug09'
# DaVinci().L0 = True
# DaVinci().ReplaceL0BanksWithEmulated = True
##############################################################################
#
# DaVinci
#
DaVinci().DataType = "2009"                   # Default is "MC09"
DaVinci().UserAlgorithms = [ dstWriter.sequence() ]
DaVinci().EvtMax = 10000
##############################################################################
#
# Real Data
#
importOptions( "$DAVINCIROOT/options/2009-RealData-Reco05.py" )
##############################################################################

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#from Configurables import CondDB
#CondDB(UseOracle = True)
