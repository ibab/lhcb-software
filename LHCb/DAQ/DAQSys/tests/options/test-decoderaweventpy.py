from Gaudi.Configuration import *
from Configurables import LHCbApp, DataOnDemandSvc
from Configurables import GaudiSequencer
from Configurables import EventClockSvc

importOptions("$STDOPTS/DecodeRawEvent.py")

LHCbApp()
EventClockSvc(EventTimeDecoder = "OdinTimeDecoder")

mySeq=GaudiSequencer("Decoding")
mySeq.OutputLevel=VERBOSE

def append():
    for loc,alg in DataOnDemandSvc().AlgMap.iteritems():
        if ('UT' not in loc) and ('FT' not in loc) and ("VL" not in loc) and ("VP" not in loc):
            if alg not in mySeq.Members:
                 mySeq.Members.append(alg)

#make sure ODIN is first, to resolve ST dependencies manually...
mySeq.Members=["createODIN"]
append()
ApplicationMgr().TopAlg=[mySeq]
