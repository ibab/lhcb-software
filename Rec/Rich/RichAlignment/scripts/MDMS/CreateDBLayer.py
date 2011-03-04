#! /usr/bin/env python

import CondDBUI
from PyCool import cool
import os
import datetime

def getUNIXTime(dtime):
    import time
    t = time.mktime(dtime.timetuple())
    return int( t * 1e9 )

def genXML(root,cond):
    file = open(root+cond)
    data = ""
    for line in file : data += line
    file.close()
    return data

def addToDB(startTime,rootToFiles,condPath,db):
    start = getUNIXTime(startTime) 
    stop  = cool.ValidityKeyMax
    db.storeXMLString( condPath, genXML(rootToFiles,condPath), start, stop )

# Path to alignment files. Expects xml files in the usual areas underneath these directories
dbPath = "/usera/jonesc/NFS/DetDB/MDMS"

# The CondDB paths to fill
condPaths  = [ "/Conditions/Rich1/Environment/HPD.xml",
               "/Conditions/Rich2/Environment/HPD.xml" ]

# Open a new DB file
dbFileName = "NewMDMSCondDB.db"
if os.path.exists(dbFileName) : os.remove(dbFileName)
db = CondDBUI.CondDB( "sqlite_file:"+dbFileName+"/LHCBCOND",
                      create_new_db=True, readOnly=False )
print "Created DB", dbFileName
for path in condPaths : db.createNode(path)

# Fill the DB slice
start = datetime.datetime(  2009,  9,     1,    1,     0,   0  )
for path in condPaths : addToDB(start,dbPath,path,db)
