from Gaudi.Configuration import *
"""
Options for building Stripping
"""
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Build the streams and stripping object
#
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingArchive.Utils import buildStreams, cloneLinesFromStream, cloneStream

streams = buildStreams('Stripping20r1')

#
# Now do some cloning to deal with the charm full/microDST streams
#
leptonicMicroDSTname = 'Leptonic'
charmMicroDSTname = 'Charm'
charmCopyDSTname  = 'CharmCompleteEvent'
calibrationFullDSTname = 'Calibration'


from Configurables import ProcStatusCheck
filterBadEvents = ProcStatusCheck()

sc = StrippingConf( Streams = streams,
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

from DSTWriters.microdstelements import *
from DSTWriters.Configuration import (SelDSTWriter,
                                      stripDSTStreamConf,
                                      stripDSTElements,
                                      stripMicroDSTStreamConf,
                                      stripMicroDSTElements)

#
# Configuration of MicroDST
#
mdstStreamConf = stripMicroDSTStreamConf()
mdstElements = stripMicroDSTElements()

#
# Configuration of Calibration stream
# to include additional track containers
#
copyExtraTrackData = MoveObjects(objects = [ "Rec/TrackEffMuonTT_SelMuonTTPParts/ProtoParticles",
                                             "Rec/TrackEffMuonTT_SelMakeMuonTT/Tracks",
          "Rec/Downstream/Tracks",
          "Rec/Downstream/FittedTracks",
          "Rec/ProtoP/DownMuonTrackEffDownMuonNominalProtoPMaker/ProtoParticles",
          "Rec/VeloMuon/Tracks",
          "Rec/ProtoP/VeloMuonTrackEffVeloMuonProtoPMaker/ProtoParticles" ])


calibElements = stripDSTElements() + [ copyExtraTrackData ]

#
# Configuration of SelDSTWriter
#
SelDSTWriterElements = {
    'default'              : stripDSTElements(),
    charmMicroDSTname      : mdstElements,
    leptonicMicroDSTname   : mdstElements,
    calibrationFullDSTname : calibElements
    }


SelDSTWriterConf = {
    'default'              : stripDSTStreamConf(),
    charmMicroDSTname      : mdstStreamConf,
    leptonicMicroDSTname   : mdstStreamConf,
    calibrationFullDSTname : stripDSTStreamConf()
    }


dstWriter = SelDSTWriter( "MyDSTWriter",
                          StreamConf = SelDSTWriterConf,
                          MicroDSTElements = SelDSTWriterElements,
                          OutputFileSuffix ='000000',
                          SelectionSequences = sc.activeStreams()
                          )

#
# DaVinci Configuration
#
from Configurables import DaVinci
DaVinci().InputType = 'SDST'
DaVinci().DataType = "2011"
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().HistogramFile = "DVHistos.root"
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )

#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py")


from Configurables import DaVinci
DaVinci().EvtMax=-1
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:../options/TestSDSTCatalog.xml']
