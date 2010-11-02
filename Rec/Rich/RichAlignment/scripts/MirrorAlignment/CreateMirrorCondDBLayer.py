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
#upAlign = "/usera/jonesc/NFS/DetDB/BestDB/Up"
#dnAlign = "/usera/jonesc/NFS/DetDB/BestDB/Down"
upAlign = "/usera/jonesc/NFS/DetDB/CurrentDB/Up"
dnAlign = "/usera/jonesc/NFS/DetDB/CurrentDB/Down"

# The following dates are extracted from spreadsheets here
# http://marwww.in2p3.fr/~legac/LHCb/

# Hardcode the field changes. Format is date of change and the new polarity
field = { }
#                          Year  Month  Day   Hour   Min  Sec
#field[ datetime.datetime(  2009,  9,     1,    1,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  4,     5,    2,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  4,     6,    2,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  5,     8,   22,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  5,    14,   12,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  5,    15,   16,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  5,    19,    5,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  7,    13,    4,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  7,    28,   21,     0,   0  ) ] = dnAlign
#field[ datetime.datetime(  2010,  8,    18,    8,     0,   0  ) ] = upAlign
#field[ datetime.datetime(  2010,  8,    29,   17,     0,   0  ) ] = dnAlign
field[ datetime.datetime(  2010,  9,    22,   16,     0,   0  ) ] = upAlign
field[ datetime.datetime(  2010,  10,   24,    9,     0,   0  ) ] = dnAlign 

# The CondDB paths to fill
condPaths  = [ "/Conditions/Rich1/Alignment/Mirrors.xml",
               "/Conditions/Rich2/Alignment/Mirrors.xml" ]
#condPaths += [ "/Conditions/Rich1/Alignment/HPDsP0.xml",
#               "/Conditions/Rich1/Alignment/HPDsP1.xml",
#               "/Conditions/Rich2/Alignment/HPDsP0.xml",
#               "/Conditions/Rich2/Alignment/HPDsP1.xml"
#               ]
condPaths += [ "/Conditions/Rich1/Alignment/SiSensorsP0.xml",
               "/Conditions/Rich1/Alignment/SiSensorsP1.xml",
               "/Conditions/Rich2/Alignment/SiSensorsP0.xml",
               "/Conditions/Rich2/Alignment/SiSensorsP1.xml"
               ]

# Open a new DB file
dbFileName = "NewMirrorHPDAlignFieldPolarity.db"
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
