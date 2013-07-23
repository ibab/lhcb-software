import DAQSys
from DAQSys.Decoders import decoder_db
from DAQSys.DecoderClass import validate

validate(decoder_db)

for k,v in decoder_db.items():
    if v.Active:
        if len(v.listInputs())==0:
            raise ValueError("Decoder "+v.FullName+" input location unknown.")
        for l in v.listInputs():
            if not len(l):
                raise ValueError("Zero length default for input "+v.FullName)
        if len(v.listOutputs())==0:
            raise ValueError("Decoder "+v.FullName+" output location unknown.")
        for l in v.listOutputs():
            if not len(l):
                raise ValueError("Zero length default for output "+v.FullName)
        if len(v.Banks)==0:
            raise ValueError("Decoder "+v.FullName+" banks unknown.")

#check the setup
for k,decoder in decoder_db.items():
    if decoder.Active:
        decoder.setup()

#check the configurable
from Configurables import DecodeRawEvent
DecodeRawEvent().OverrideInputs="Hlt"
DecodeRawEvent().DataOnDemand=True
DecodeRawEvent().__apply_configuration__()

print "Pass"
