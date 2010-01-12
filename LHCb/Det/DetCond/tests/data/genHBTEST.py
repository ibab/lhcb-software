#!/usr/bin/env python
"""
Small script to generate the test database for the heart beat test.
(kept for reference)
"""

from CondDBUI import CondDB
from PyCool import cool

base_time = 1262304000 # (2010, 1, 1, 0, 0, 0, 4, 1, 0) GMT
unit = 60

db = CondDB("sqlite_file:HBTEST.db/HBTEST", readOnly = False, create_new_db = True)

db.createNode("/Conditions", storageType = "NODE")
db.createNode("/Conditions/Online", storageType = "NODE")
db.createNode("/Conditions/Online/HeartBeatTest", storageType = "NODE")
db.createNode("/Conditions/Online/HeartBeatTest/Condition1", versionMode = "SINGLE")
db.createNode("/Conditions/Online/HeartBeatTest/Condition2", versionMode = "SINGLE")
db.createNode("/Conditions/Online/HeartBeatTest/Tick", versionMode = "SINGLE")

cond = """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>
  <condition name = "Condition%d">
    <param name = "Data" type = "int"> %d </param>
  </condition>
</DDDB>
"""

db.storeXMLString("/Conditions/Online/HeartBeatTest/Condition1", cond % (1, 0), 0, cool.ValidityKeyMax)
for t in [20, 40]:
    db.storeXMLString("/Conditions/Online/HeartBeatTest/Condition1", cond % (1, t), (base_time + t * unit) * 1000000000, cool.ValidityKeyMax)

db.storeXMLString("/Conditions/Online/HeartBeatTest/Condition2", cond % (2, 0), 0, cool.ValidityKeyMax)
for t in [20, 40, 80]:
    db.storeXMLString("/Conditions/Online/HeartBeatTest/Condition2", cond % (2, t), (base_time + t * unit) * 1000000000, cool.ValidityKeyMax)

hb = """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>
<condition name="Tick">
<param name="Alive" type="int">1</param>
</condition>
</DDDB>
"""
db.storeXMLString("/Conditions/Online/HeartBeatTest/Tick", hb, 0, cool.ValidityKeyMax)
db.storeXMLString("/Conditions/Online/HeartBeatTest/Tick", hb, (base_time + 60 * unit) * 1000000000, cool.ValidityKeyMax)
