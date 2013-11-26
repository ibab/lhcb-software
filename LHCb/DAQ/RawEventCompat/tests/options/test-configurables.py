from Gaudi.Configuration import *
from Configurables import RawEventFormatConf, RecombineRawEvent
from Configurables import RawEventJuggler, RawEventMapCombiner
from Configurables import bankKiller
from Configurables import DataOnDemandSvc as dods
from Configurables import GaudiSequencer as GS
from Configurables import InputCopyStream as ics
from Configurables import OutputStream as os

test_locations={
    0.0 : {
        "Bank_A" : "/Foo",
        "Bank_B" : "/Bar",
        "Bank_C" : "FooBar/#TCK#/",
        "Bank_D" : "/Bar",
        "Bank_E" : "/Bar", #nowhere to copy from, print a warning
        "Bank_H" : "/SpamAndEggs"
        },
    99.0 : {
        "Bank_A" : "FooBar", #killed as an extra node
        "Bank_B" : ["FooBar","Spam"],#killed a extra nodes
        "Bank_C" : "FooBar", #killed before and after
        "Bank_D" : "Foo/#BAR#", #killed as an extra node
        "Bank_F" : "Foo/#BAR#",
        "Bank_G" : "/SpamAndEggs",  #killed as an extra bank
        "Bank_H" : "/SpamAndEggs"
        }
    }

test_versions={"Spam" : 0.0, "Eggs" : 99.0 }

RawEventFormatConf().Locations=test_locations

RawEventFormatConf().RecoDict=test_versions


#test RecombineRawEvent with all options
RecombineRawEvent().Version="Eggs"
RecombineRawEvent().Method="Map"
RecombineRawEvent().Regex=".*(_A|_B)"
RecombineRawEvent().__apply_configuration__()

expected_combiner=RawEventMapCombiner("resurectRawEventMap")

if "DAQ/RawEvent" not in dods().AlgMap or dods().AlgMap["DAQ/RawEvent"]!=expected_combiner:
    raise ValueError("Failed to add to DOD")

if expected_combiner.RawBanksToCopy!={'Bank_A' : 'FooBar', 'Bank_B' : "FooBar"}:
    print expected_combiner.RawBanksToCopy
    raise ValueError("MapCombiner misconfigured")


#test RawEventJuggler with all options
RawEventJuggler().Sequencer=GS("JuggleRawEvent")
RawEventJuggler().Input="Eggs"
RawEventJuggler().Output=0.0
RawEventJuggler().KillExtraNodes=True
RawEventJuggler().KillExtraBanks=True
RawEventJuggler().KillInputBanksBefore=".*_C"
RawEventJuggler().KillInputBanksAfter=".*_C"
RawEventJuggler().WriterOptItemList=ics("WriteAsOptItems")
RawEventJuggler().WriterItemList=os("WriteAsItems")
RawEventJuggler().TCK="Null"
RawEventJuggler().GenericReplacePatterns={"#BAR#":"Bar"}
RawEventJuggler().__apply_configuration__()

if [ent.getFullName() for ent in GS("JuggleRawEvent").Members] != ['bankKiller/kill_FooBar_Before', 'RawEventMapCombiner/create_Foo', 'RawEventMapCombiner/create_Bar', 'RawEventMapCombiner/create_FooBar_Null', 'bankKiller/kill_SpamAndEggs_After', 'bankKiller/kill_FooBar_After', 'EventNodeKiller/KillRawEventNodes']:
    print [ent.getFullName() for ent in GS("JuggleRawEvent").Members]
    raise ValueError("Configuration order or members are wrong")

if ics("WriteAsOptItems").OptItemList!=os("WriteAsItems").ItemList or ics("WriteAsOptItems").OptItemList!=['/Event/Foo#1', '/Event/Bar#1', '/Event/SpamAndEggs#1', '/Event/FooBar/Null#1']:
    print ics("WriteAsOptItems").OptItemList, os("WriteAsItems").ItemList
    raise ValueError("Output locations are wrong")

if RawEventMapCombiner("create_Bar").RawBanksToCopy!={'Bank_D': 'Foo/Bar', 'Bank_B': 'FooBar'} or RawEventMapCombiner("create_Bar").OutputRawEventLocation!="/Bar":
    print RawEventMapCombiner("create_Bar").RawBanksToCopy, RawEventMapCombiner("create_Bar").OutputRawEventLocation
    raise ValueError("Unexpected raw bank locations")

if EventNodeKiller("KillRawEventNodes").Nodes!=['Foo/Bar', 'Spam', 'FooBar']:
    print EventNodeKiller("KillRawEventNodes").Nodes
    raise AttributeError("EventNodeKiler misconfigured")

if [(bankKiller(name).RawEventLocations, bankKiller(name).BankTypes) for name in ["kill_SpamAndEggs_After","kill_FooBar_After"]]!=[(['/SpamAndEggs'], ['Bank_G']), (['FooBar'], ['Bank_C'])]:
    print [(bankKiller(name).RawEventLocations, bankKiller(name).BankTypes) for name in ["kill_SpamAndEggs_After","kill_FooBar_After"]]
    raise AttributeError("Kill Sequences misconfigured")

print "Pass"

