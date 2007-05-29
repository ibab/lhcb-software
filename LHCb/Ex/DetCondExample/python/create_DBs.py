#!/usr/bin/env python

from PyCool import cool
import os, sys

dbs = cool.DatabaseSvcFactory.databaseService()

spec = cool.FolderSpecification(cool.FolderVersioning.MULTI_VERSION)
spec.payloadSpecification().extend("data",cool.StorageType.String16M)
payload = cool.Record(spec.payloadSpecification())

xml_templ = '<?xml version="1.0" encoding="UTF-8"?>' + \
            '<!DOCTYPE DDDB SYSTEM "structure.dtd">' + \
            '<DDDB><condition name="%s"><param name="Database" type="other">%s</param></condition></DDDB>'

db = dbs.createDatabase(os.environ["CONDDBCONNECTIONSTRING1"])
f = db.createFolder("/multiDBTest/Folder1",
                    spec,"<storage_type=7>",True)
payload["data"] = xml_templ%("Cond1","DB1")
f.storeObject(0, 10, payload, 0)


db = dbs.createDatabase(os.environ["CONDDBCONNECTIONSTRING2"])
f = db.createFolder("/multiDBTest/Folder1",
                    spec,"<storage_type=7>",True)
payload["data"] = xml_templ%("Cond1","DB2")
f.storeObject(0, 20, payload, 0)

f = db.createFolder("/multiDBTest/Folder2",
                    spec,"<storage_type=7>",True)
payload["data"] = xml_templ%("Cond2","DB2")
f.storeObject(0, 20, payload, 0)

f = db.createFolder("/multiDBTest/Folder3",
                    spec,"<storage_type=7>",True)
payload["data"] = xml_templ%("Cond2","DB2")
f.storeObject(0, 20, payload, 0)

payload["data"] = '<?xml version="1.0" encoding="UTF-8"?>' + \
            '<!DOCTYPE DDDB SYSTEM "structure.dtd">' + \
            '<DDDB><condition classID="6" name="Align1">' + \
            '<paramVector name="dPosXYZ" type="double">0. 0. 1.</paramVector>' + \
            '<paramVector name="dRotXYZ" type="double">0. 0. 0.</paramVector></condition></DDDB>'
f.storeObject(0, 10, payload, 0)
payload["data"] = '<?xml version="1.0" encoding="UTF-8"?>' + \
            '<!DOCTYPE DDDB SYSTEM "structure.dtd">' + \
            '<DDDB><condition classID="6" name="Align1">' + \
            '<paramVector name="dPosXYZ" type="double">0. 1. 0.</paramVector>' + \
            '<paramVector name="dRotXYZ" type="double">0. 0. 0.</paramVector></condition></DDDB>'
f.storeObject(10, 20, payload, 0)

db = dbs.createDatabase(os.environ["DAQCONNECTIONSTRING"])
