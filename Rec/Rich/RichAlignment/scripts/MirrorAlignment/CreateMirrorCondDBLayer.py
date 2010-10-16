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
upAlign = "/usera/jonesc/NFS/DetDB/BestDB/Up"
dnAlign = "/usera/jonesc/NFS/DetDB/BestDB/Down"

# The following dates are extracted from spreadsheets here
# http://marwww.in2p3.fr/~legac/LHCb/

# Hardcode the field changes. Format is date of change and the new polarity
field = { }
#                          Year  Month  Day   Hour   Min  Sec
field[ datetime.datetime(  2009,  9,     1,    0,     0,   0  ) ] = dnAlign
field[ datetime.datetime(  2010,  4,     5,    2,    10,   8  ) ] = upAlign
field[ datetime.datetime(  2010,  4,     6,    3,     8,  49  ) ] = dnAlign
field[ datetime.datetime(  2010,  5,     8,   22,    53,  53  ) ] = upAlign
field[ datetime.datetime(  2010,  5,    14,   13,     7,  21  ) ] = dnAlign
field[ datetime.datetime(  2010,  5,    15,   17,    20,  36  ) ] = upAlign
field[ datetime.datetime(  2010,  5,    19,    6,    40,   8  ) ] = dnAlign
field[ datetime.datetime(  2010,  7,    13,    5,    18,  49  ) ] = upAlign
field[ datetime.datetime(  2010,  7,    28,   22,    37,  11  ) ] = dnAlign
field[ datetime.datetime(  2010,  8,    18,    9,    21,  32  ) ] = upAlign
field[ datetime.datetime(  2010,  8,    29,   18,    26,   8  ) ] = dnAlign
field[ datetime.datetime(  2010,  9,    22,   17,     1,  39  ) ] = upAlign
field[ datetime.datetime(  2010,  10,   12,   23,     59, 59  ) ] = dnAlign # To be checked later on

# The CondDB paths to fill
condPaths = [ "/Conditions/Rich1/Alignment/Mirrors.xml",
              "/Conditions/Rich2/Alignment/Mirrors.xml" ]

# Open a new DB file
dbFileName = "NewMirrorAlignFieldPolarity.db"
if os.path.exists(dbFileName) : os.remove(dbFileName)
db = CondDBUI.CondDB( "sqlite_file:"+dbFileName+"/LHCBCOND",
                      create_new_db=True, readOnly=False )
print "Created DB", dbFileName
for path in condPaths: db.createNode(path)

# Loop over the data and fill the DB slice
for start in sorted(field.keys()):
    align = field[start]
    print start, align
    for path in condPaths: addToDB(start,align,path,db)

