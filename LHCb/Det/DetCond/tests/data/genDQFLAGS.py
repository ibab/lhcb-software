#!/usr/bin/env python
"""
Small script to generate the test database for the heart beat test.
(kept for reference)
"""

from CondDBUI import CondDB
from PyCool import cool
from datetime import datetime, timedelta

def toTimeStamp(dt):
    t = dt - datetime(1970, 1, 1, 0)
    return (t.days * 60 * 60 * 24 + t.seconds) * 1000000000

db = CondDB("sqlite_file:DQFLAGS.db/DQFLAGS", readOnly = False, create_new_db = True)

db.createNode("/Conditions", storageType = "NODE")
db.createNode("/Conditions/DQ", storageType = "NODE")
db.createNode("/Conditions/DQ/Flags", versionMode = "SINGLE")

def cond(d):
    data = ['<item key="%s" value="%d"/>' % i for i in d.items()]
    return """<?xml version='1.0' encoding='ISO-8859-1'?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>
  <condition name="Flags">
    <map keytype="string" name="map" valuetype="int">
      %s
    </map>
  </condition>
</DDDB>
""" % ("\n      ".join(data))

for since, until, d in [(datetime(2012, 1, 1, 0), datetime(2012, 1, 2, 0), {"DET1": 1}),
                        (datetime(2012, 1, 2, 0), datetime(2012, 1, 3, 0), {"DET2": 1}),
                        (datetime(2012, 1, 3, 0), datetime(2012, 1, 4, 0), {}),
                        (datetime(2012, 1, 5, 0), datetime(2012, 1, 6, 0), {"DET3": 1})]:
    since, until = map(toTimeStamp, (since, until))
    db.storeXMLString("/Conditions/DQ/Flags", cond(d), since, until)
