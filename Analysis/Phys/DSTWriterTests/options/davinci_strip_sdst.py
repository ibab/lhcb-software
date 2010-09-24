from Gaudi.Configuration import *
from Configurables import DaVinci

from StrippingConf.Configuration import StrippingConf

from StrippingSettings.Sept2010 import StreamCalibration
from StrippingSettings.Sept2010 import StreamBhadron
from StrippingSettings.Sept2010 import StreamCharm
from StrippingSettings.Sept2010 import StreamDielectron
from StrippingSettings.Sept2010 import StreamDimuon
from StrippingSettings.Sept2010 import StreamMiniBias
from StrippingSettings.Sept2010 import StreamSemileptonic
from StrippingSettings.Sept2010 import StreamRadiative
from StrippingSettings.Sept2010 import StreamEW


allStreams = [
               StreamCalibration.stream, 
               StreamBhadron.stream,
               StreamCharm.stream,
               StreamDielectron.stream,
               StreamDimuon.stream,
               StreamMiniBias.stream,
               StreamSemileptonic.stream,
               StreamRadiative.stream,
               StreamEW.stream,
             ]

sc = StrippingConf( Streams = allStreams,
                    MaxCandidates = 2000 )

charm = StreamCharm.stream.name()

from DSTWriters.__dev__.microdstelements import *
from DSTWriters.__dev__.Configuration import (SelDSTWriter,
                                              stripDSTStreamConf,
                                              stripDSTElements,
                                              microDSTStreamConf,
                                              microDSTElements)

extraMicroElements = [CloneLHCbIDs(),
                      ReFitAndClonePVs(),
                      CloneRawBanks(banks = ['HltSelReports',
                                             'HltDecReports']),
                      CloneHltDecReports( locations = [ 'Strip/Phys/DecReports' ] ),
                      CloneL0DUReport()
                      ]



streamConf = {'default' : stripDSTStreamConf(),
              charm     : microDSTStreamConf()}

elements = {'default' : stripDSTElements(),
            charm     : microDSTElements() + extraMicroElements }

conf = SelDSTWriter("StripDST",
                    StreamConf = streamConf,
                    MicroDSTElements = elements,
                    OutputFileSuffix = "01234567",
                    SelectionSequences = sc.activeStreams())


conf.StreamConf[charm] =  microDSTStreamConf()

selDST0Seq = conf.sequence()

#

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = 2000
#dv.UserAlgorithms = [multiSeqA.sequence(), multiSeqB.sequence(), selDST0Seq]
dv.UserAlgorithms = [conf.sequence()]
importOptions("Reco05-Stripping09_SDSTs.py")
#EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
#FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
