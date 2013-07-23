from Gaudi.Configuration import *
from Configurables import DecodeRawEvent, LHCbApp
from Configurables import GaudiSequencer

LHCbApp()
mySeq=GaudiSequencer("Decoding")
DecodeRawEvent().Sequencer=mySeq
ApplicationMgr().TopAlg+=mySeq
