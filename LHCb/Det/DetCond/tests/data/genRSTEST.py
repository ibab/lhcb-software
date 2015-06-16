#!/usr/bin/env python
"""
Small script to generate the test database for the run stamp test.
(kept for reference)
"""

from CondDBUI import CondDB
from PyCool import cool

def ts(*args):
    from datetime import datetime
    epoch = datetime(1970, 1, 1)
    return int((datetime(*args) - epoch).total_seconds() * 1000000000)

db = CondDB("sqlite_file:RSTEST.db/RSTEST", readOnly = False, create_new_db = True)

db.createNode("/Conditions", storageType = "NODE")
db.createNode("/Conditions/Online", storageType = "NODE")
db.createNode("/Conditions/Online/LHCb", storageType = "NODE")
db.createNode("/Conditions/Online/LHCB/RunInfo", versionMode = "SINGLE")
db.createNode("/Conditions/Online/LHCb/RunStamp.xml", versionMode = "SINGLE")

rs = """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>
<condition name="RunStamp">
<param name="RunNumber" type="int">1</param>
</condition>
</DDDB>
"""
db.storeXMLString("/Conditions/Online/LHCb/RunStamp.xml", rs, ts(2015, 6, 9), ts(2015, 6, 10))
db.storeXMLString("/Conditions/Online/LHCb/RunStamp.xml", rs, ts(2015, 6, 11), ts(2015, 6, 12))
