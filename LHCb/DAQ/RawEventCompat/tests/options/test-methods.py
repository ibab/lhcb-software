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
        "Bank_C" : "FooBar"
        }
    }

test_versions={"Spam" : 0.0, "Eggs" : 99.0 }

conf.RawEventFormatConf().Locations=test_locations

conf.RawEventFormatConf().RecoDict=test_versions

loc,reco=conf._getDict()

if loc!=test_locations or reco!=test_versions:
    raise ValueError("Couldn't set up custom dictionaries")

for version in test_versions:
    conf._checkv(version)

for version in test_locations:
    conf._checkv(version)

if conf.WhereAll("Bank_B",99.0)!=test_locations[99.0]["Bank_B"]:
    raise ValueError("WhereAll method fails")


if conf.WhereBest("Bank_B",99.0)!=test_locations[99.0]["Bank_B"][0]:
    raise ValueError("WhereBest method fails")

for version in test_versions:
    revdict=conf.ReverseDict(version)
    for bank in test_locations[test_versions[version]]:
        locs=test_locations[test_versions[version]][bank]
        if type(locs) is not list:
            locs=[locs]
        for aloc in locs:
            if aloc not in revdict:
                print revdict
                raise KeyError("ReverseDict doesn't work as expected")

conf._dod("SPAM")
from Configurables import DataOnDemandSvc as dods
if "DAQ/RawEvent" not in dods().AlgMap or dods().AlgMap["DAQ/RawEvent"]!="SPAM":
    raise ValueError("_dod method fails")

print "Pass"
