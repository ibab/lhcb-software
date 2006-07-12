#!/usr/bin/env python

from PyCool import cool, coral
import os, sys

dbs = cool.DatabaseSvcFactory.databaseService()
dbs.createDatabase(os.environ["CONDDBCONNECTIONSTRING"])

spec = cool.ExtendedAttributeListSpecification()
spec.push_back("data", "string", cool.PredefinedStorageHints.STRING_MAXSIZE_16M)

payload = coral.AttributeList()
payload.extend("data","string")

xml_templ = '<?xml version="1.0" encoding="UTF-8"?>' + \
            '<!DOCTYPE DDDB SYSTEM "structure.dtd">' + \
            '<DDDB><condition name="%s"><param name="Database" type="other">%s</param></condition></DDDB>'

db = dbs.createDatabase(os.environ["CONDDBCONNECTIONSTRING1"])
f = db.createFolder("/multiDBTest/Folder1",
                    spec,"<storage_type=7>",
                    cool.FolderVersioning.MULTI_VERSION,True)
payload["data"] = xml_templ%("Cond1","DB1")
f.storeObject(0, 10, payload)


db = dbs.createDatabase(os.environ["CONDDBCONNECTIONSTRING2"])
f = db.createFolder("/multiDBTest/Folder1",
                    spec,"<storage_type=7>",
                    cool.FolderVersioning.MULTI_VERSION,True)
payload["data"] = xml_templ%("Cond1","DB2")
f.storeObject(0, 20, payload)

f = db.createFolder("/multiDBTest/Folder2",
                    spec,"<storage_type=7>",
                    cool.FolderVersioning.MULTI_VERSION,True)
payload["data"] = xml_templ%("Cond2","DB2")
f.storeObject(0, 20, payload)
