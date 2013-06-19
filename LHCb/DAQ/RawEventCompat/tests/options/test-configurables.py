from Gaudi.Configuration import *
from Configurables import RawEventFormatConf, RecombineRawEvent
from Configurables import RawEventJuggler, RawEventMapCombiner
from Configurables import DataOnDemandSvc as dods
from Configurables import GaudiSequencer as GS
from Configurables import InputCopyStream as ics
from Configurables import OutputStream as os

test_locations={
    0.0 : {
        "Bank_A" : "/Foo",
        "Bank_B" : "/Bar",
        "Bank_C" : "FooBar/#TCK#/",
        "Bank_D" : "/Bar"
        },
    99.0 : {
        "Bank_A" : "FooBar",
        "Bank_B" : ["FooBar","Spam"],
        "Bank_C" : "FooBar",
        "Bank_D" : "Foo/#BAR#"
        }
    }

test_versions={"Spam" : 0.0, "Eggs" : 99.0 }

RawEventFormatConf().Locations=test_locations

RawEventFormatConf().RecoDict=test_versions


#test RecombineRawEvent with all options
RecombineRawEvent().Version=99.0
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
RawEventJuggler().Input=99.0
RawEventJuggler().Output=0.0
RawEventJuggler().KillExtraNodes=True
RawEventJuggler().KillInputBanksBefore=".*_C"
RawEventJuggler().KillInputBanksAfter=".*_C"
RawEventJuggler().WriterOptItemList=ics("WriteAsOptItems")
RawEventJuggler().WriterItemList=os("WriteAsItems")
RawEventJuggler().TCK="Null"
RawEventJuggler().GenericReplacePatterns={"#BAR#":"Bar"}
RawEventJuggler().__apply_configuration__()

if [ent.getFullName() for ent in GS("JuggleRawEvent").Members] != ['bankKiller/killBeforeFooBar', 'RawEventMapCombiner/createFoo', 'RawEventMapCombiner/createBar', 'RawEventMapCombiner/createFooBarNull', 'bankKiller/killAfterFooBar', 'EventNodeKiller/KillRawEventNodes']:
    print [ent.getFullName() for ent in GS("JuggleRawEvent").Members]
    raise ValueError("Configuration order or members are wrong")

if ics("WriteAsOptItems").OptItemList!=os("WriteAsItems").ItemList or ics("WriteAsOptItems").OptItemList!=['/Foo#1', '/Bar#1', 'FooBar/Null/#1']:
    print ics("WriteAsOptItems").OptItemList, os("WriteAsItems").ItemList
    raise ValueError("Output locations are wrong")

if RawEventMapCombiner("createBar").RawBanksToCopy!={'Bank_D': 'Foo/Bar', 'Bank_B': 'FooBar'} or RawEventMapCombiner("createBar").OutputRawEventLocation!="/Bar":
    print RawEventMapCombiner("createBar").RawBanksToCopy, RawEventMapCombiner("createBar").OutputRawEventLocation
    raise ValueError("Unexpected raw bank locations")

print "Pass"
