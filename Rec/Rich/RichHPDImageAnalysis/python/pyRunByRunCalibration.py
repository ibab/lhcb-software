
from Gaudi.Configuration import *

def initialise():
    
    from Configurables import DDDBConf, CondDB, CondDBAccessSvc, LHCbApp
    
    # Initialise a few things
    DDDBConf(DataType = "2010")
    LHCbApp().DDDBtag   = "head-20100518"
    LHCbApp().CondDBtag = "head-20100730"
    CondDB()
    
    import GaudiPython
    GaudiPython.AppMgr().initialize()

def detSvc():
    import GaudiPython
    return GaudiPython.AppMgr().detSvc()

def umsSvc():
    import GaudiPython
    GaudiPython.AppMgr().createSvc('UpdateManagerSvc')
    return GaudiPython.AppMgr().service('UpdateManagerSvc','IUpdateManagerSvc')

def richSystem():
    return detSvc()["Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem"]

def rootFileListFromTextFile(rootFileList='RootFileNames.txt'):

    # Open the text file
    files = open(rootFileList,'r')

    # list of root files
    rootfiles = [ ]

    # Read file list from text file
    for file in files.read().split('\n') :
        if len(file) > 0 :
            rootfiles += [file]
            
    # Sort the list
    rootfiles.sort()
    
    # Close the file
    files.close()

    # Return the file list
    return rootfiles

def getHPD(copyNumber):

    # Get RichSmartID from copy number
    smartID = richSystem().richSmartID(copyNumber)

    # XML Location of HPD
    hpdLoc = richSystem().getDeHPDLocation(smartID)

    # return the HPD
    return detSvc()[hpdLoc]

def getHPDAlignment(copyNumber) : return getAlignment(copyNumber,"HPD")

def getSiSensorAlignment(copyNumber) : return getAlignment(copyNumber,"SiSensor")

def getAlignment(copyNumber,type):

    from GaudiPython import gbl

    rich = ["Rich1","Rich2"]

    # Get RichSmartID from copy number
    smartID = richSystem().richSmartID(copyNumber)

    # Path to alignment condition
    alignLoc = "/dd/Conditions/Alignment/"+rich[smartID.rich()]+"/"+type+str(copyNumber.data())+"_Align"

    # get the aligonment condition
    align = detSvc()[alignLoc]

    # Force an update
    umsSvc().invalidate(align)
    align.forceUpdateMode()

    # return a copy (so any changes do not affect the originals)
    return gbl.AlignmentCondition(align)

def alignmentFilePath(copyNumber):

    rich  = ["Rich1","Rich2"]

    # RichSmartID
    smartID = richSystem().richSmartID(copyNumber)
    
    return "/Conditions/"+rich[smartID.rich()]+"/Alignment/HPDsAndSiSensorsP"+str(smartID.panel())+".xml"

def xmlHeader(run):
    return """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>

<!-- HPD Image alignment for run """ + str(run) + """ -->
"""

def xmlFooter():
    return """
</DDDB>"""

def hpdXMLComment(copyNumber):
    smartID = richSystem().richSmartID(copyNumber)
    hardID  = richSystem().hardwareID(smartID)
    return """
<!-- HPD hardwareID: H"""+str(hardID.data())+"""  Copy Number: """+str(copyNumber.data())+""" -->
"""

# Get the run number from file name (ugly, but works ...)
def getRunNumber(filename):
    run = 0
    split = ''
    splits = filename.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == 'Run' : run = s[1]
    return run.split('.')[0]

def getUNIXTime(dtime):
    import time
    t = time.mktime(dtime.timetuple())
    return int( t * 1e9 )

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

def getRunTimes(files):

    import datetime, time, os

    # Database API
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    database = BookkeepingClient()

    times = { }
    times["RunTimes"] = { }
    times["GlobalStopTime"] = None

    runList = [ ]
    for filename in files:
        run = getRunNumber(filename)
        if run not in runList : runList += [run]
    runList.sort()

    # Loop over the sorted run list and get the times
    tmpTime = 0
    for run in runList:
        # Get run start and stop times
        res = database.getRunInformations(int(run))
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
            import DIRAC
            DIRAC.exit(1)

    # Return the Run Time Information
    return times

def runCalibration(rootfiles='RootFileNames.txt'):

    import pyHistoParsingUtils
    from ROOT import TFile
    import GaudiPython
    from GaudiPython import gbl
    import CondDBUI
    import datetime, time, os
    from PyCool import cool
        
    # Load the list of root files
    files = rootFileListFromTextFile(rootfiles)

    # Number of HPDs
    #nHPDs = 1
    nHPDs = 484

    # Open new alignment SQL slice
    dbFileName = "NewRichHPDAlignments.db"
    if os.path.exists(dbFileName) : os.remove(dbFileName)
    db = CondDBUI.CondDB( "sqlite_file:"+dbFileName+"/LHCBCOND",
                          create_new_db=True, readOnly=False )

    # Get the run time info
    runsTimes = getRunTimes(files)

    # List of paths already created in the DB
    createdPaths = [ ]

    # Loop over the root files and fit the HPD images
    for filename in files:

        # Open the root file
        file = TFile(filename)

        # Run number
        run = getRunNumber(filename)
        print "Processing Run", run, "(", filename, ")"

        # Alignment data files
        alignments = { }

        # Get run start/stop time
        unixStartTime = getUNIXTime(runsTimes["RunTimes"][run]["Start"])
        unixStopTime  = getUNIXTime(runsTimes["RunTimes"][run]["Stop"])

        # Set UMS to the time for this run
        #umsSvc().newEvent( gbl.Gaudi.Time(unixStartTime) )
        umsSvc().newEvent()

        for hpdID in range(0,nHPDs):

            # Get the HPD for this copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)

            # Get the alignment conditions
            siAlign  = getSiSensorAlignment(copyNumber)
            hpdAlign = getHPDAlignment(copyNumber)
     
            # Get the offsets. Use try to catch errors
            try:
                
                xOff = pyHistoParsingUtils.imageOffsetX(file,hpdID)
                yOff = pyHistoParsingUtils.imageOffsetY(file,hpdID)

                # Update the Si alignment with the image movement data
                #print "Aligning HPD", hpdID
                paramName = "dPosXYZ"
                vect = siAlign.paramAsDoubleVect(paramName)
                vect[0] = xOff
                vect[1] = yOff
                vect[2] = 0
                siAlign.addParam( paramName, vect, "" )
                vect = siAlign.paramAsDoubleVect(paramName)
            
                # Double check update is correct
                if vect[0] != xOff or vect[1] != yOff :
                    print "Warning :  Update mismatch ", xOff, yOff, ":", vect[0], vect[1]
                
            except Exception,e:
                
                #print "No alignment update possible for HPD", hpdID
                pass

            # The alignment path for the HPD
            alignPath = alignmentFilePath(copyNumber)

            # Get alignment XML file
            if alignPath not in alignments.keys() : alignments[alignPath] = xmlHeader(run)

            # Add alignments for this HPD
            alignments[alignPath] += hpdXMLComment(copyNumber)
            alignments[alignPath] += hpdAlign.toXml() + '\n' # Soon not needed
            alignments[alignPath] += siAlign.toXml()  + '\n'

        # close the ROOT file
        file.Close()

        # Update the DB with the HPD alignments for the IOV for this run
        startTime = correctStartTime( run, unixStartTime )
        stopTime  = cool.ValidityKeyMax
        for xmlpath in alignments.keys():
            # The XML data
            data = alignments[xmlpath]
            # Add the XML footer to the XML data
            data += xmlFooter()
            # Add to the DB
            if xmlpath not in createdPaths:
                db.createNode(xmlpath)
                createdPaths += [xmlpath]
            db.storeXMLString( xmlpath, data, startTime, stopTime )
