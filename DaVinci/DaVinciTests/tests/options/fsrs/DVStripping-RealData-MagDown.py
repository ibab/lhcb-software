from Gaudi.Configuration import *

from CommonParticles.Utils import DefaultTrackingCuts

from Configurables import DaVinci

from StrippingConf.Configuration import StrippingConf
from StrippingArchive.Utils import buildStreams

allStreams = buildStreams('Stripping24')

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

sc = StrippingConf( Streams = allStreams )

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import (SelDSTWriter,
                                              stripDSTStreamConf,
                                              stripDSTElements,
                                              stripMicroDSTStreamConf,
                                              stripMicroDSTElements,
                                              stripCalibMicroDSTStreamConf
                                              )


SelDSTWriterElements = {
    'default'              : stripDSTElements(),
    }


SelDSTWriterConf = {
    'default'              : stripDSTStreamConf(),
    }

dstWriter = SelDSTWriter( "MyDSTWriter",
                          StreamConf = SelDSTWriterConf,
                          MicroDSTElements = SelDSTWriterElements,
                          OutputFileSuffix ='MagDown',
                          SelectionSequences = sc.activeStreams()
                          )


DaVinci().EvtMax = 10                        # Number of events
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )
