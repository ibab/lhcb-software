from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter
from StrippingConf.Configuration import StrippingConf

from StrippingSelections.Streams import StreamDimuon, StreamCalibration
sc = StrippingConf( Streams = [StreamDimuon.stream] )

from DSTWriters.__dev__.Configuration import StripDSTWriter

dstWriter = StripDSTWriter("StripDSTWriter",
                           SelectionSequences = sc.activeStreams(),
                           OutputFileSuffix = 'TestDiMuon',
                           )

dv = DaVinci()
dv.DataType = '2010'
dv.InputType='SDST'
dv.Simulation = True
dv.EvtMax = 5000
dv.appendToMainSequence( [ sc.sequence() ] )
dv.appendToMainSequence( [ dstWriter.sequence() ] )
#dv.UserAlgorithms = [dstWriter.sequence()]
importOptions("Reco05-Stripping09_SDSTs.py")

