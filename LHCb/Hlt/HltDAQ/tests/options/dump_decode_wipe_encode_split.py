from Gaudi.Configuration import *
from Configurables import bankKiller, RawEventDump, GaudiSequencer
from Configurables import LHCbApp

import os
if os.path.exists("rewritten.mdf"):
    print "EXISTS"
    os.system("rm rewritten.mdf")
else:
    print "NOTHING!"

LHCbApp().EvtMax=10
dump=RawEventDump(RawBanks=[],DumpData=True)
decoders=GaudiSequencer("Decode",Members=[])
killer=bankKiller(BankTypes=[])
killer2=bankKiller("Killer2",BankTypes=[])
encoders=GaudiSequencer("Encode",Members=[])

import Configurables
from Configurables import IODataManager
IODataManager().DisablePFNWarning=True

locs = {"Decoder" : "Output", "Writer" : "Input"}

#can't do track just yet, can be added easily, though later
bankTypes = ["Dec","Sel","Vertex"]

from itertools import product
for stage, t, (seq, at) in product((1, 2), bankTypes, ((decoders, "Decoder"), (encoders, "Writer"))):
    algType = "Hlt{0}Reports{1}".format(t, at)
    algName = "Hlt{0}{1}Reports{2}".format(stage, t, at)
    conf = getattr(Configurables, algType)(algName)
    conf.SourceID = stage
    conf.setProp("{0}Hlt{1}ReportsLocation".format(locs[at], t), "Hlt{0}/{1}Reports".format(stage, t))
    seq.Members.append(conf.getFullName())

for atype in bankTypes:
    bank="Hlt"+atype+"Reports"
    dump.RawBanks.append(bank)
    killer.BankTypes.append(bank)

from Configurables import HltANNSvc
HltANNSvc().Hlt1SelectionID = {
    'PV3D'                                  : 10103,
    'ProtoPV3D'                             : 10117
    }
HltANNSvc().Hlt2SelectionID = {
    'PV3D'                                  : 50103,
    'ProtoPV3D'                             : 50117
}

killer2.BankTypes=killer.BankTypes+["ODIN"]
killer2.DefaultIsKill=True

ApplicationMgr().TopAlg+=[dump,decoders,killer,encoders,killer2]

#writer
from GaudiConf import IOHelper
from Configurables import LHCb__MDFWriter
IOHelper('MDF','MDF').outStream("rewritten.mdf",LHCb__MDFWriter("writer",Compress=0),writeFSR=False)

#inputfile
from PRConfig import TestFileDB
TestFileDB.test_file_db["2012_raw_default"].run()
from GaudiConf import IOExtension
IOExtension().inputFiles(["../options/OnlyHltBanks.mdf"],clear=True)
