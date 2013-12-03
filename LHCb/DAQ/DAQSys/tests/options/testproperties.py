from DAQSys.Decoders import DecoderDB
l0du=DecoderDB["L0DUFromRawTool"]
l0du.Properties["SpammySpamSpam"] = False

wasset=[]
for k,d in DecoderDB.iteritems():
    if d==l0du:
        continue
    if "SpammySpamSpam" in d.Properties:
        wasset.append(k)

if len(wasset):
    raise KeyError("Setting a property also accidentally set a property for" + wasset.__str__())

print "Pass"
