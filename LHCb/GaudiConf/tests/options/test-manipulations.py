from functools import partial

#test the import
from GaudiConf.Manipulations import *

#create some algs from Gaudi, ready to have their properties set
from Configurables import InputCopyStream, EventCounter
ics=InputCopyStream(name="I_Created_This")
ec=EventCounter(name="I_Also_Created_This")

if nameFromConfigurable(ics)!="InputCopyStream/I_Created_This":
    raise ValueError("Name found is not correct",nameFromConfigurable(ics))


if configurableInstanceFromString(nameFromConfigurable(ics))!=ics:
    raise ValueError("the configurable instance wasn't the same as I created")

ops="OutputStream/You_Created_This"

if nameFromConfigurable(configurableInstanceFromString(ops))!=ops:
    raise ValueError("I wasn't able to create the configurable requested")

if (ops not in fullNameConfigurables()) or (nameFromConfigurable(ics) not in fullNameConfigurables()) or (nameFromConfigurable(ec) not in fullNameConfigurables()):
    raise ValueError("I couldn't find the configurables we just created, "+fullNameConfigurables().__str__())

ics.OutputLevel=2

recurseConfigurables(
    partial(setPropertiesAndAddTools, properties={'OutputLevel':3,"OutputFile":'ASpamName'}),
    fullNameConfigurables().values()
)

if not ec.isPropertySet("OutputLevel"):
    raise AttributeError("I didn't set the OutputLevel of the EventCounter you made")
if ics.getProp("OutputLevel")!=2:
    raise AttributeError("I set the output level of ics when it was already set elsewhere")
for conf in [configurableInstanceFromString(ops),ics]:
    if (not conf.isPropertySet("OutputFile")) or conf.getProp("OutputFile")!='ASpamName':
        raise ("I didn't correctly set the property of output file for "+nameFromConfigurable(conf))

fdict={'OutputLevel':7,"OutputFile":'AnotherSpamName'}

recurseConfigurables(
    partial(setPropertiesAndAddTools, properties=fdict, force=True),
    fullNameConfigurables().values()
)

for conf in [configurableInstanceFromString(ops),ics,ec]:
    for key in fdict:
        if hasattr(conf,key) or (hasattr(conf,"properties") and key in conf.properties()) or(hasattr(conf,"__slots__") and key in conf.__slots__):
            if (not conf.isPropertySet(key)) or conf.getProp(key)!=fdict[key]:
                raise ("I didn't correctly set the property of" +key+" for "+nameFromConfigurable(conf))

#check that I follow down sequencers!
gs=configurableInstanceFromString("GaudiSequencer/testSeq")
gs.Members=["EventCounter/NewCounter","InputCopyStream/newStream","OutputStream/spamStream"]

recurseConfigurables(
    partial(setPropertiesAndAddTools, properties=fdict, force=True),
    fullNameConfigurables().values(),
    descend_properties=['Members']
)

for conf in gs.Members:
    conf=configurableInstanceFromString(conf)
    for key in fdict:
        if hasattr(conf,key) or (hasattr(conf,"properties") and key in conf.properties()) or(hasattr(conf,"__slots__") and key in conf.__slots__):
            if (not conf.isPropertySet(key)) or conf.getProp(key)!=fdict[key]:
                raise ("I didn't correctly set the property of" +key+" for "+nameFromConfigurable(conf))

print "PASS"
