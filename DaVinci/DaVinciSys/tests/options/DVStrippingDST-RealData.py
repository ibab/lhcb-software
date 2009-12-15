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
from StrippingSelections import StreamBmuon, StreamHadron, StreamJpsi, StreamDstar, StreamLambda, StreamBelectron
sc = StrippingConf( Streams = [ StreamBmuon.stream
                    , StreamHadron.stream 
                    , StreamJpsi.stream
                    , StreamDstar.stream
                    , StreamLambda.stream
#                    , StreamBelectron.stream 
                    ] )
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
DaVinci().Input   = [
"   DATAFILE='PFN:/castor/cern.ch/user/s/smenzeme/DATA2009/run63683-63690_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'",
"   DATAFILE='PFN:/castor/cern.ch/user/p/pkoppenb/DATA2009/run63445-63497_RecoStatus20091210.dst' TYP='POOL_ROOTTREE' OPT='READ'"
] 
##############################################################################

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"
#from Configurables import CondDB
#CondDB(UseOracle = True)
