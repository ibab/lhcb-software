# Test single line of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
stream.appendLines( [ StrippingBd2DstarMuNuLooseConf().line() ] )
sc = StrippingConf( Streams = [ stream ] )

from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
dvinit = GaudiSequencer("DaVinciInitSeq")
dvinit.Members.insert(0, redoPV() )
dvinit.Members.insert(0, veloNZSKiller() )

DaVinci().PrintFreq = 100
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000 
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
DaVinci().Input = [
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002181_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'", 
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002182_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'",
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002183_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'", 
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002199_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'", 
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002294_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'", 
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002308_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'", 
"DATAFILE='LFN:/lhcb/data/2010/SDST/00006563/0000/00006563_00002347_1.sdst' TYP='POOL_ROOTTREE' OPT='READ'" 
]
FileCatalog().Catalogs= ["xmlcatalog_file:pool_xml_catalog.xml"]

