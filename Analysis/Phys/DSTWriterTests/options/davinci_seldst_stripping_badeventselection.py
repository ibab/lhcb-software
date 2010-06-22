from Gaudi.Configuration import *
from Configurables import DaVinci, SelDSTWriter, DeterministicPrescaler
from StrippingConf.Configuration import StrippingConf, StrippingStream, StrippingLine
from StrippingSelections.StrippingBd2KstarMuMu import StrippingBd2KstarMuMuConf
from PhysSelPython.Wrappers import AutomaticData

pions = AutomaticData(Location = 'Phys/StdLoosePions')

eventFilter = DeterministicPrescaler('BadEventFilter', AcceptFraction = 0.5)
line = StrippingLine('PionLine', algos = [pions])
line1 = StrippingLine('PionLine2', algos = [pions])

streamDefault = StrippingStream("PionDefault", Lines = [line])
streamDefault1 = StrippingStream("PionDefault1", Lines = [line1])
streamBadEvent = StrippingStream("PionBadEvent", Lines = [line], BadEventSelection = eventFilter)


sc = StrippingConf( Streams = [streamDefault, streamDefault1, streamBadEvent] ) 

dstWriter = SelDSTWriter("SelDST0", 
                         OutputPrefix = 'SequenceName',
                         OutputFileSuffix = "Test2",
                         SelectionSequences = sc.activeStreams())

dv = DaVinci()
dv.DataType = '2010'
dv.InputType = 'SDST'
dv.EvtMax = -1
dv.appendToMainSequence( [ sc.sequence() ] )
dv.appendToMainSequence( [ dstWriter.sequence() ] )
EventSelector().Input = ["DATAFILE='PFN:castor:/castor/cern.ch/user/c/cattanem/testFiles/Brunel-v37r1-069857_0000000006-1000ev.sdst' TYP='POOL_ROOTTREE' OPT='READ'"]
FileCatalog().Catalogs =['xmlcatalog_file:TestSDSTCatalog.xml']
