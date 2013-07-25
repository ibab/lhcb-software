from Gaudi.Configuration import *
from Configurables import LHCbApp, DataOnDemandSvc
from Configurables import GaudiSequencer

importOptions("$STDOPTS/DecodeRawEvent.py")

LHCbApp()
mySeq=GaudiSequencer("Decoding")
mySeq.OutputLevel=VERBOSE

def append():
    for loc,alg in DataOnDemandSvc().AlgMap.iteritems():
        if ('UT' not in loc) and ('FT' not in loc) and ("VL" not in loc) and ("VP" not in loc):
            if alg not in mySeq.Members:
                 mySeq.Members.append(alg)

append()
ApplicationMgr().TopAlg=[mySeq]
