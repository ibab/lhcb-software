from DAQSys.Decoders import DecoderDB as ddb
from DAQSys.DecoderClass import decodersForBank
from Configurables import GaudiSequencer, DecodeRawEvent, DataOnDemandSvc

mySeq=GaudiSequencer("DecodeTest")
mySeq.Members+=[d.setup() for d in decodersForBank(ddb,"Velo")]

newDec=ddb["OTTimeCreator"].clone("OTTimeCreator/Ot2")
newDec.Properties["OutputLevel"]=42
newDec.overrideInputs("Other/RawEvent")
newDec.overrideOutputs("Other/OTTimes")

DecodeRawEvent().OverrideInputs="Strip20"
DecodeRawEvent().DataOnDemand=True

DecodeRawEvent().__apply_configuration__()

if "Other/OTTimes" not in DataOnDemandSvc().AlgMap:
    raise KeyError("Other/OTTimes not correctly added to DataOnDemand")

print "Pass"
