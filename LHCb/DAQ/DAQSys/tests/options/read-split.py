from Gaudi.Configuration import *
from Configurables import DecodeRawEvent

from DAQSys.Decoders import DecoderDB

for i,v in DecoderDB.iteritems():
    v.Properties["OutputLevel"]=VERBOSE
    if "Hlt" in i and "ReportsDecoder" in i:
        v.Active=False
        #v.Inputs={"InputRawEventLocation":"DAQ/RawEvent"}
    for b in ["UT","FT","FTCluster","VP","VL"]:
        if b in v.Banks:
            v.Active=False

DecoderDB["MuonRec"].Active=False

DecodeRawEvent().OverrideInputs=999
from Configurables import GaudiSequencer
#DecodeRawEvent().Sequencer=GaudiSequencer("SPAM")

for i,v in DecoderDB.iteritems():
    if v.Active:
       GaudiSequencer("SPAM").Members.append(v.setup())

from Configurables import StoreExplorerAlg

StoreExplorerAlg().Load=True

ApplicationMgr().TopAlg=[GaudiSequencer("SPAM"),StoreExplorerAlg()]
