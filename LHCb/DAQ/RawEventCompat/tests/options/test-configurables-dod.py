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



#test RawEventJuggler with all options
RawEventJuggler().Input=99.0
RawEventJuggler().Output=0.0
RawEventJuggler().TCK="Null"
RawEventJuggler().GenericReplacePatterns={"#BAR#":"Bar"}
RawEventJuggler().DataOnDemand=True
RawEventJuggler().__apply_configuration__()

if [(node,dods().AlgMap[node].getFullName()) for node in dods().AlgMap]!=[('/Foo', 'RawEventMapCombiner/create_Foo'), ('/Bar', 'RawEventMapCombiner/create_Bar'), ('FooBar/Null/', 'RawEventMapCombiner/create_FooBar_Null')]:
    print [(node,dods().AlgMap[node].getFullName()) for node in dods().AlgMap]
    raise ValueError("Unexpected Node Map in DoD Svc")


print "Pass"
