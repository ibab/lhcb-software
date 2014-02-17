import DAQSys
from DAQSys.DecoderClass import Decoder
from DAQSys.DecoderClass import validate

#test features of the Decoder class

test_db={}

thednames=["GaudiSequencer/SomeCrazyAlg","GaudiSequencer/SomeOtherCrazyAlg","GaudiSequencer/Acounter","GaudiSequencer/FishSticks","GaudiSequencer/FishSticks2"]

d1=Decoder(thednames[0],
           active=True, banks=["Barclays"],
           properties={"Members":["Something","AnotherThing"]},
           conf=test_db)

d2=Decoder(thednames[1],
           active=True, banks=["Lloyds"],
           properties={"OutputLevel":7},
           inputs={"Members":["Blah","Blah"]},
           outputs={"Members":["Blah","Blah"]},
           conf=test_db)

d3=Decoder(thednames[2],
           active=True, banks=["TSB"],
           properties={"OutputLevel":7},
           outputs={"Members":["Blah","Blah"]},
           publicTools=[thednames[3]],
           conf=test_db)

d4=Decoder(thednames[3],
           active=False,
           properties={"OutputLevel":6},
           inputs={"Members":["Blah","Blah"]},
           conf=test_db)

d5=Decoder(thednames[4],
           active=True,
           properties={"OutputLevel":6},
           inputs={"Members":None},
           conf=test_db)

validate(test_db)

#some simple checks!
if d1.isInputSettable():
    raise ValueError("Input is found to be settable! 1")

if not d2.isInputSettable():
    raise ValueError("Input not found to be settable! 2")

if not d3.isInputSettable():
    raise ValueError("Input not found to be settable! 3")

if not d4.isInputSettable():
    raise ValueError("Input not found to be settable! 45")

if not d5.isInputSettable():
    raise ValueError("Input not found to be settable! ")


if d1.isOutputSettable():
    raise ValueError("Output is found to be settable! 1")

if not d2.isOutputSettable():
    raise ValueError("Output not found to be settable! 2")

if not d3.isOutputSettable():
    raise ValueError("Output not found to be settable! 3")

if d4.isOutputSettable():
    raise ValueError("Output found to be settable! 4")

if d5.isOutputSettable():
    raise ValueError("Output found to be settable! 5")

if d5.listOutputs()!=[]:
    print d5.listInputs()
    raise AttributeError("List outputs failed. ")

confds=[]
for tool in [d1,d2,d3,d5]:
    confds.append(tool.setup())

#Check some simple things
if confds[0].getFullName()!=thednames[0]:
    raise NameError("Wrong Name! "+thednames[0])
if confds[1].getFullName()!=thednames[1]:
    raise NameError("Wrong Name! "+thednames[1])
if confds[2].getFullName()!=thednames[2]:
    raise NameError("Wrong Name! "+thednames[2])
if confds[3].getFullName()!=thednames[4]:
    raise NameError("Wrong Name! "+thednames[4])

#now construct some configurables myself, to check they have had their properties set correctly.

from Configurables import GaudiSequencer
myC=GaudiSequencer(thednames[3].split('/')[-1])
if not myC.isPropertySet("OutputLevel") or not myC.isPropertySet("Members") or myC.getProp("OutputLevel")!=6 or myC.getProp("Members")!=["Blah","Blah"]:
    print myC.Properties()
    raise AttributeError("properties not cascaded to configurable! 1")

#test that I can override the input options with cascading
d3.overrideInputs(["And","Now","For","Something"])
d5.overrideInputs(["Completely","Different"])
confds=[]
for tool in [d1,d2,d3,d5]:
    confds.append(tool.setup())

if confds[3].getProp("Members")!=["Completely","Different"]:
    print confds[3].getProp("Members")
    raise AttributeError("properties not cascaded to configurable! 2")

if not myC.isPropertySet("OutputLevel") or not myC.isPropertySet("Members") or myC.getProp("OutputLevel")!=6 or myC.getProp("Members")!=["And","Now","For","Something"]:
    print myC.getProp("OutputLevel"), myC.getProp("Members")
    raise AttributeError("properties not cascaded to configurable! 3")

if d3.listInputs()!=["And","Now","For","Something"]:
    print d3.listInputs()
    raise AttributeError("List inputs failed. ")

if d2.listOutputs()!=["Blah"]:
    print d3.listInputs()
    raise AttributeError("List outputs failed. ")


#now validate the whole DB
from DAQSys.Decoders import DecoderDB
validate(DecoderDB)

#setup a new databse with tree of interdependencies

test_db={}

#          0
#
#    1        7 9  10
#  /   \      |/
#  2    3     8
#  |    |\
#  4   /  5
#   \ /
#    6
#

#Should run as 10 897 563421 0

for n in range(11):
    name=str(n)
    Decoder(name,
            active=True,
            banks=[name],
            inputs=[name],
            outputs=[name],
            conf=test_db)

test_db["1"].Required=["2","3"]
test_db["2"].Required=["4"]
test_db["3"].Required=["6","5"]
test_db["4"].Required=["6"]
test_db["7"].Required=["8"]
test_db["9"].Required=["8"]
test_db["0"].Required=["1","7","9","10"]

validate(test_db)

if test_db["0"].listRequired()!=['10', '8', '9', '7', '5', '6', '3', '4', '2', '1']:
    print test_db["0"].listRequired()
    raise KeyError("Failed to flatten the tree correctly")

from DAQSys.Decoders import caloSetZeroSuppressed
caloSetZeroSuppressed(DecoderDB,False)
#next add configuration of the inactive algorithms, because they're not in the default DB
for dec in ["L0CaloAlg/L0Calo","HltTrackReportsDecoder/Hlt1TrackReportsDecoder","CaloDigitsFromRaw/EcalFromRaw","CaloDigitsFromRaw/HcalFromRaw"]:
    if dec not in DecoderDB:
        raise KeyError("The decoder called "+dec+" isn't in the database any more, update options/test-decoder-db.py")
    DecoderDB[dec].Active=True
    validate(DecoderDB)
    DecoderDB[dec].setup()

print "Pass"
