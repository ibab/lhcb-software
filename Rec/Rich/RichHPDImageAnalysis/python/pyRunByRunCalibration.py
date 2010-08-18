

tempRootDir = "/var/work/jonesc/tmp"

def initialise():
    
    from Configurables import DDDBConf, CondDB, CondDBAccessSvc, LHCbApp
    
    # Initialise a few things
    DDDBConf(DataType = "2010")
    LHCbApp().DDDBtag   = "head-20100518"
    LHCbApp().CondDBtag = "head-20100730"
    CondDB()
    
    import GaudiPython
    GaudiPython.AppMgr().initialize()

def iDataSvc():
    import GaudiPython
    return GaudiPython.AppMgr().detSvc()

def umsSvc():
    import GaudiPython
    GaudiPython.AppMgr().createSvc('UpdateManagerSvc')
    return GaudiPython.AppMgr().service('UpdateManagerSvc','IUpdateManagerSvc')

def iDetDataSvc():
    import GaudiPython
    #GaudiPython.AppMgr().createSvc('DetectorDataSvc')
    return GaudiPython.AppMgr().service('DetectorDataSvc','IDetDataSvc')

def richSystem():
    return iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem"]

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
    return iDataSvc()[hpdLoc]

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
    align = iDataSvc()[alignLoc]

    # Force an update
    umsSvc().invalidate(align)
    align.forceUpdateMode()

    # Return the alignment condition
    return align

def siliconAlignmentFilePath(copyNumber):

    rich = ["Rich1","Rich2"]

    # RichSmartID
    smartID = richSystem().richSmartID(copyNumber)
    
    return "/Conditions/"+rich[smartID.rich()]+"/Alignment/SiSensorsP"+str(smartID.panel())+".xml"

def xmlHeader(type,flag):
    return """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>

<!-- HPD Image alignment for """ + str(type) + """ """ + str(flag) + """ -->
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
def getIntInfo(filename,type):
    run = 0
    split = ''
    splits = filename.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == type : run = s[1]
    return int(run.split('.')[0])

def getUNIXTime(dtime):
    import time
    t = time.mktime(dtime.timetuple())
    return int( t * 1e9 )

def correctStartTime(time):
    startTimeOffset = int( 5 * 1e9 )
    return time - startTimeOffset

def getRunFillData(files):

    import datetime, time, os

    # Database API
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    database = BookkeepingClient()

    runfilldata = { }
    runfilldata["RunData"] = { }
    runfilldata["FillData"] = { }
    runfilldata["GlobalStopTime"] = None

    # Loop over the sorted run list and get the runfilldata
    tmpTime = 0
    for filename in files:
        run = getIntInfo(filename,'Run')
        # Get run start and stop times
        res = database.getRunInformations(int(run))
        if res['OK'] :
            start = res['Value']['RunStart']
            stop  = res['Value']['RunEnd']
            fill  = int(res['Value']['FillNumber'])
            runfilldata["RunData"][run] = { "Start" : start, "Stop" : stop, "Fill" : fill }
            if fill not in runfilldata["FillData"].keys():
                runfilldata["FillData"][fill] = { "Start" : start, "Stop" : stop, "Files" : []  }
            fillData = runfilldata["FillData"][fill]
            if fillData["Start"] > start : fillData["Start"] = start
            if fillData["Stop"] < stop   : fillData["Stop"]  = stop
            fillData["Files"] += [filename]
            print "Run", run, "is from", start, "to", stop, "Fill", fill
            unixEndTime = getUNIXTime( stop )
            if unixEndTime > tmpTime :
                tmpTime = unixEndTime
                runfilldata["GlobalStopTime"] = stop
        else:
            print "ERROR Getting start/stop times for run", run
            import DIRAC
            DIRAC.exit(1)

    # Return the Run Time Information
    return runfilldata

def mergeRootFile(fill,infiles):

    import os

    # Create merged ROOT files for given fill data
    mergedfile = globals()["tempRootDir"]+"/HPDAlign_Fill-"+str(fill)+".root"
    if os.path.exists(mergedfile) : os.remove(mergedfile)
    command = "hadd "+mergedfile
    for infile in infiles : command += " " + str(infile)
    print command
    os.system(command)

    return mergedfile

def runToFill(run):
    # Database API
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    database = BookkeepingClient()
    res = database.getRunInformations(int(run))
    fill = 0
    if res['OK'] :
        fill = res['Value']['FillNumber']
    else:
        print "ERROR Getting Fill for run", run
        import DIRAC
        DIRAC.exit(1)
    return fill

def calibrationByRuns(rootfiles='RootFileNames.txt'):
    return calibration(rootfiles,'Run')

def calibrationByFills(rootfiles='RootFileNames.txt'):
    return calibration(rootfiles,'Fill')

def calibration(rootfiles,type):

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
    dbFileName = "NewRichHPDAlignmentsBy"+type+".db"
    if os.path.exists(dbFileName) : os.remove(dbFileName)
    db = CondDBUI.CondDB( "sqlite_file:"+dbFileName+"/LHCBCOND",
                          create_new_db=True, readOnly=False )

    # Get the run/fill info
    runFillData = getRunFillData(files)

    # List of paths already created in the DB
    createdPaths = [ ]

    # If calibrating by Fill, group and merge files
    filesToLoopOver = files
    if type == 'Fill' :
        filesToLoopOver = [ ]
        for fill in sorted(runFillData["FillData"].keys()):
            infiles = runFillData["FillData"][fill]["Files"]
            filesToLoopOver += [mergeRootFile(fill,infiles)]

    # Loop over the root files and fit the HPD images
    for filename in filesToLoopOver:

        # Open the root file
        file = TFile(filename)

        # Run/fill number
        flag = getIntInfo(filename,type)

        # Alignment data files
        alignments = { }

        # Get run/fill start and stop time
        startTime = runFillData[type+"Data"][flag]["Start"]
        stopTime  = runFillData[type+"Data"][flag]["Stop"]
        unixStartTime = getUNIXTime(startTime)
        unixStopTime  = getUNIXTime(stopTime)

        print "Processing", type , flag, "(", filename, ")"
        print " ->", type, "Start", startTime, "Stop", stopTime

        # Set UMS to the start time for this run/fill
        iDetDataSvc().setEventTime( gbl.Gaudi.Time(unixStartTime) )
        umsSvc().newEvent()

        for hpdID in range(0,nHPDs):

            # Get the HPD for this copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)

            # Get the alignment conditions
            siAlign  = getSiSensorAlignment(copyNumber)
            #hpdAlign = getHPDAlignment(copyNumber)

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

            # The alignment path for the HPD silicon
            alignPath = siliconAlignmentFilePath(copyNumber)

            # Get alignment XML file
            if alignPath not in alignments.keys() : alignments[alignPath] = xmlHeader(type,flag)

            # Add alignments for this HPD
            alignments[alignPath] += hpdXMLComment(copyNumber)
            #alignments[alignPath] += hpdAlign.toXml() + '\n' # Not needed with new DB
            alignments[alignPath] += siAlign.toXml()  + '\n'

        # close the ROOT file
        file.Close()
        os.remove(filename)

        # Update the DB with the HPD alignments for the IOV for this run/fill
        startTime = correctStartTime( unixStartTime )
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
