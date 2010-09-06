#! /usr/bin/env python

import CondDBUI
from PyCool import cool
import os
import pickle
import datetime, time
import DIRAC

def genXML(data,run):
    return """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>

<!-- Run """+str(run)+""" -->

<condition name="RefractivityScaleFactor">
<param name="CurrentScaleFactor" type="double">"""+str(data)+""" </param>
</condition>

</DDDB>"""

def getUNIXTime(dtime):
    import time
    t = time.mktime(dtime.timetuple())
    return int( t * 1e9 )

def getCalibrationsFromFile(rad):
    
    # Load the Corrections
    filename = rad+'Gas-RefIndexCalib.pck'
    print "Loading (n-1) Calibrations for", rad, "from", filename

    # Unpickle the calibration data
    file = open(filename,"r")
    calibrations = pickle.load(file)

    # Close file and return the data
    file.close()
    return calibrations

def correctStartTime(run,time):
    # Runs before 72908 incorrectly did not have their secs stored, so correct by 60s
    # Otherwise use 5 secs
    retTime = 0
    if run < 72908 :
        startTimeOffset = int( 60 * 1e9 )
        retTime = time - startTimeOffset
    else:
        startTimeOffset = int( 5 * 1e9 )
        retTime = time - startTimeOffset
    return retTime

def pickleDict(filename,data):
    import pickle, os
    file = open(filename,"w")
    pickle.dump(data,file)
    file.close()

def loadDict(filename):
    import pickle, os
    data = { }
    if os.path.exists(filename) :
        file = open(filename,"r")
        data = pickle.load(file)
        file.close()
    return data

def fillDB(calibration,db,runsTimes,rad):

    # Create DB paths for the conditions
    path = "/Conditions/"+rad+"/Environment/Gas.xml"
    db.createNode(path)

    # Add a null entry covering all periods
    #db.storeXMLString( path, genXML(1.0,0), cool.ValidityKeyMin, cool.ValidityKeyMax )

    # Loop over runs to fill the conditions
    for run in sorted(calibration.keys()):
        scaleF = calibration[run]
        
        # Run IOV
        dStartTime = runsTimes["RunTimes"][run]["Start"]
        #dStopTime  = runsTimes["RunTimes"][run]["Stop"]
        dStopTime  = runsTimes["GlobalStopTime"]
        
        startTime = correctStartTime( run, getUNIXTime(dStartTime) )
        #stopTime  = getUNIXTime( dStopTime  )
        stopTime  = cool.ValidityKeyMax

        # Scale factor
        scale = '%g' % scaleF[0]

        # Store in the DB
        print " -> Setting", rad, "(n-1) Scale", scale, "from", dStartTime, "to", dStopTime
        db.storeXMLString( path, genXML(scale,run), startTime, stopTime )

def getRunTimes(calibrations):

    times = { }
    times["RunTimes"] = { }
    times["GlobalStopTime"] = None

    runList = [ ]
    for calibration in calibrations:
        for run in sorted(calibration.keys()):
            if run not in runList : runList += [run]
    runList.sort()
    
    # Load the raw cached run data
    RunCacheName = "RunInfoCache.pck"
    runTimeCache = loadDict(RunCacheName)

    tmpTime = 0
    cacheUpdated = False
    for run in runList:
        # Get run start and stop times
        if run in runTimeCache.keys():
            res = runTimeCache[run]
        else:
            from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
            res = BookkeepingClient().getRunInformations(int(run))
            if len(res.keys()) > 0 and res.has_key('OK') :
                if res['OK']:
                    runTimeCache[run] = res
                    cacheUpdated = True
        if res['OK'] :
            start = res['Value']['RunStart']
            stop  = res['Value']['RunEnd']
            times["RunTimes"][run] = { "Start" : start, "Stop" : stop }
            print "Run", run, "is from", start, "to", stop
            unixEndTime = getUNIXTime( stop )
            if unixEndTime > tmpTime :
                tmpTime = unixEndTime
                times["GlobalStopTime"] = stop
        else:
            print "ERROR Getting start/stop times for run", run
            DIRAC.exit(1)

    # Update the saved cache 
    if cacheUpdated : pickleDict(RunCacheName,runTimeCache)

    # Return the Run Time Information
    return times

# Open a new DB
dbFileName = "NewRichCKRefIndexCalib.db"
if os.path.exists(dbFileName) : os.remove(dbFileName)
db = CondDBUI.CondDB( "sqlite_file:"+dbFileName+"/LHCBCOND",
                      create_new_db=True, readOnly=False )

# Load the RICH1 and RICH2 calibrations from file
cali1 = getCalibrationsFromFile('Rich1')
cali2 = getCalibrationsFromFile('Rich2')

# Create a unique dict of all run start,stop times
runsTimes = getRunTimes([cali1,cali2])

# Fill the Conditions
print "Filling CondDB slice ...."
fillDB(cali1,db,runsTimes,'Rich1')
fillDB(cali2,db,runsTimes,'Rich2')
print "Done"

DIRAC.exit(0)
