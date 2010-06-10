from Gaudi.Configuration import *
from Configurables import InputCopyStream

InputCopyStream().Output = "DATAFILE='PFN:DaVinci-Merged-2010.dst' TYP='POOL_ROOTTREE' OPT='REC'"

ApplicationMgr().EvtMax = -1

from Configurables import RecordStream
FileRecords = RecordStream("FileRecords")
FileRecords.Output = "DATAFILE='PFN:DaVinci-Merged-2010.dst' TYP='POOL_ROOTTREE' OPT='REC'"
