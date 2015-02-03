from Gaudi.Configuration import *
from Configurables import LHCbAlgsTests__TestTimeDecoderOdin as TimeDecoder
from Configurables import (RunChangeHandlerSvc, DBXferAlg )
DetectorPersistencySvc( CnvServices = [ xmlCnvSvc ] )
rch = RunChangeHandlerSvc()
rch.Conditions = {"Condition": "../data/runchangehandler/run%04d.xml"}
