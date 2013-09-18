from Gaudi.Configuration import *
from Configurables import DecodeRawEvent, LHCbApp
from Configurables import GaudiSequencer

LHCbApp()
mySeq=GaudiSequencer("Decoding")
mySeq.OutputLevel=VERBOSE
DecodeRawEvent().Sequencer=mySeq
ApplicationMgr().TopAlg=[mySeq]

#deactivate Upgrade banks

from DAQSys.Decoders import DecoderDB
[DecoderDB[k].deactivate()  for k in DecoderDB if (DecoderDB[k].Active and (('FT' in k) or ('UT' in k) or ('VL' in k) or ('VP' in k)))]

#configure L0TCKs
importOptions('$L0TCK/L0DUConfig.opts')

#set event time:
from Configurables import EventClockSvc
clock=EventClockSvc()
clock.EventTimeDecoder="FakeEventTime"
clock.addTool(FakeEventTime,name="FakeEventTime")
clock.FakeEventTime.StartTime="1346336270000000000" #2012 08 30
