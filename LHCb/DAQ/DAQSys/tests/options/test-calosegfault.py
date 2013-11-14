from Gaudi.Configuration import *
from Configurables import DecodeRawEvent, LHCbApp
from Configurables import GaudiSequencer

LHCbApp()

mySeq=GaudiSequencer("Decoding")
mySeq.OutputLevel=VERBOSE
DecodeRawEvent().Sequencer=mySeq
ApplicationMgr().TopAlg=[mySeq]

from DAQSys.DecoderClass import decodersForBank
from DAQSys.Decoders import DecoderDB as ddb

#only calo banks
for k,v in ddb.iteritems():
    v.Active=False

for b in ["PrsE", "EcalE", "HcalE", "ODIN"]:
    for d in decodersForBank(ddb,b,ignoreActive=True,addRequired=True):
        d.Active=True

#configure L0TCKs
importOptions('$L0TCK/L0DUConfig.opts')

