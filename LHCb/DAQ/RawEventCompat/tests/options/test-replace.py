import RawEventCompat
import RawEventCompat.Configuration as conf

test_locations={
    0.0 : {
        "Bank_A" : "/Foo",
        "Bank_B" : "/Bar",
        "Bank_C" : "FooBar/#TCK#/"
        },
    99.0 : {
        "Bank_A" : "FooBar",
        "Bank_B" : ["FooBar","Spam"],
        "Bank_C" : "#two#FooBar"
        }
    }

test_versions={"Spam" : 0.0, "Eggs" : 99.0 }

conf.RawEventFormatConf().Locations=test_locations

conf.RawEventFormatConf().RecoDict=test_versions

loc,reco=conf._getDict()

### check I throw an error if the TCK is not given ###
flag=False
try:
  conf._replaceWrap(test_locations[0.0]["Bank_C"])
  raise ValueError("Failed to throw warning about TCK replacement not being specified")
except AttributeError:
  pass

conf.RawEventFormatConf().TCK="0x0099"
if "#TCK#" in conf._replaceWrap(test_locations[0.0]["Bank_C"]):
  print conf._replaceWrap(test_locations[0.0]["Bank_C"])
  raise ValueError("Failed to replace TCK")

rej=conf.RawEventJuggler()

rej.Input=99.0
rej.Output=0.0
from Configurables import GaudiSequencer
aseq=GaudiSequencer("MoveTCKToNewLocation")
rej.Sequencer=aseq
try:
  rej.__apply_configuration__()
  raise ValueError( "Failed to protect against unset TCK")
except AttributeError:
  pass

rej.TCK="0x0099"
rej.GenericReplacePatterns={"#two#": "Aha!"}

aseq.Members=[]

rej.__apply_configuration__()


locs=[]
for mem in aseq.Members:
  locs.append(mem.OutputRawEventLocation)
  locs=locs+[aloc for abank,aloc in mem.RawBanksToCopy.iteritems()]

for loc in locs:
  if "#" in loc:
    print locs
    raise ValueError("Should have replaced the TCK and random location, but I didn't!")

print "Pass"
