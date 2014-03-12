from Gaudi.Configuration import *
from Configurables import bankKiller, RawEventDump, GaudiSequencer
from Configurables import LHCbApp

LHCbApp().EvtMax=10

dump=RawEventDump(RawBanks=[],DumpData=True)

from Configurables import IODataManager
IODataManager().DisablePFNWarning=True

for atype in ["Dec","Sel","Vertex"]: #can't do track just yet, can be added easily, though later
    bank="Hlt"+atype+"Reports"
    dump.RawBanks.append(bank)

ApplicationMgr().TopAlg+=[dump]

#inputfile
from PRConfig import TestFileDB
TestFileDB.test_file_db["2012_raw_default"].run()
from GaudiConf import IOExtension
IOExtension().inputFiles(["rewritten.mdf"],clear=True)
