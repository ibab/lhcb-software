import sys

from Gaudi.Configuration import *
from GaudiConf.Configuration import *

from GaudiPython import AppMgr, SUCCESS, FAILURE

# Configure the writing process
from Configurables import LHCbApp
from Configurables import LHCb__RawDataCnvSvc as RawDataCnvSvc


LHCbApp().EvtMax = 100

EventSelector().Input = [ "DATA='file:///data/bfys/lhcb/MinBias-L0xHlt1strip/Sim03Reco02-Mbias_0x00011409.raw' SVC='LHCb::MDFSelector'" ]

EventDataSvc().RootCLID    = 1
EventPersistencySvc().CnvServices.append( RawDataCnvSvc('RawDataCnvSvc') )

from Configurables import LHCb__MDFWriter as MDFWriter
writer = MDFWriter( 'MDFWriter',
                    Compress = 0,
                    ChecksumType = 1,
                    GenerateMD5 = True,
                    Connection = "file:copy.raw"
                    )

writer.OutputLevel = INFO
ApplicationMgr().OutStream = [ writer ]

# Initialize the application manager
## appMgr = AppMgr()
## appMgr.initialize()

# Run the events
## appMgr.run( 100 )

# Exit
## appMgr.exit()
