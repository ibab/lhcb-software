from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter, DeterministicPrescaler, FilterDesktop
from StrippingConf.Configuration import StrippingConf, StrippingStream, StrippingLine
from PhysSelPython.Wrappers import AutomaticData, SelectionSequence, Selection, MergedSelection

pions = AutomaticData(Location = 'Phys/StdLoosePions')
eventFilter = DeterministicPrescaler('BadEventFilter', AcceptFraction = 0.5)

lineX = StrippingLine('PionLineX', algos = [pions])
lineY = StrippingLine('PionLineY', algos = [pions])

streamX = StrippingStream("StreamPionX", Lines = [lineX])
streamXBadEvent = StrippingStream("StreamPionXBadEvent", Lines = [lineX], BadEventSelection = eventFilter)
streamYBadEvent = StrippingStream("StreamPionYBadEvent", Lines = [lineY], BadEventSelection = eventFilter)

sc = StrippingConf( Streams = [streamX, streamYBadEvent, streamXBadEvent] ) 

dstWriter = SelDSTWriter("SelDST0", 
                         OutputFileSuffix = "Test3",
                         SelectionSequences = sc.activeStreams())

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
#dv.appendToMainSequence( [ sc.sequence() ] )
dv.appendToMainSequence( [ dstWriter.sequence() ] )
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
