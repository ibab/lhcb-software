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

sc = StrippingConf( Streams = allStreams )
##############################################################################
#
# Hlt
#
# DaVinci().Hlt = True
# DaVinci().HltThresholdSettings = 'Physics_320Vis_300L0_10Hlt1_Jan10'
# DaVinci().L0 = True
# DaVinci().ReplaceL0BanksWithEmulated = True
##############################################################################
#
# DaVinci
#
DaVinci().DataType = "2009"                   # Default is "MC09"
DaVinci().UserAlgorithms = [ sc.sequence() ]
DaVinci().EvtMax = 1000
##############################################################################
#
# Real Data
#
importOptions( "$DAVINCIROOT/options/2009-RealData-Reco07.py" )   # change
##############################################################################

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#from Configurables import CondDB
#CondDB(UseOracle = True)
