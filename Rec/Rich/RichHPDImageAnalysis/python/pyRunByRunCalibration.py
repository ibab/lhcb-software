
# Globals
imageFileName = ''
canvas        = None
tempRootDir   = "/var/work/jonesc/tmp"

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

def rFromXY(a,b):

    from math import sqrt

    R = sqrt(a[0]*a[0]+b[0]*b[0])

    tempA = a[0]*a[0]*a[1]*a[1] + b[0]*b[0]*b[1]*b[1]
    tempB = a[0]*a[0] + b[0]*b[0]
    error = sqrt( tempA / tempB )

    return (R,error)

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
    from ROOT import TFile, TGraphErrors
    import GaudiPython
    from GaudiPython import gbl
    import CondDBUI
    import datetime, time, os
    from PyCool import cool
    from math import sqrt
        
    # Load the list of root files
    files = rootFileListFromTextFile(rootfiles)

    # Number of HPDs
    #nHPDs = 1
    nHPDs = 484

    # Min number of entries in HPD alignment histogram for update
    minHPDEntries = 10

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

    # Save shift data for plots
    plotData = { }

    # Save last 'good' alignment conditions for each HPD copy number
    lastGoodAlignment = { }

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

        # Loop over all HPD copy IDs
        for hpdID in range(0,nHPDs):

            # Get the HPD for this copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)

            # Get the alignment conditions
            siAlign  = getSiSensorAlignment(copyNumber)
            #hpdAlign = getHPDAlignment(copyNumber)
                
            # Get the offsets. Use try to catch errors
            updateOK = True
            try:

                xOff = pyHistoParsingUtils.imageOffsetX(file,hpdID,minHPDEntries)
                yOff = pyHistoParsingUtils.imageOffsetY(file,hpdID,minHPDEntries)

                if hpdID not in plotData.keys() : plotData[hpdID] = { }
                plotData[hpdID][flag] = { "ShiftR"    : rFromXY(xOff,yOff),
                                          "ShiftX"    : xOff,
                                          "ShiftY"    : yOff
                                          }

                # Save as last good alignment for this HPD
                lastGoodAlignment[hpdID] = [xOff,yOff]
                
            except Exception,e:

                #print "No good fit for this", type
                if hpdID in lastGoodAlignment.keys():
                    #print " -> Using last good alignment", lastGoodAlignment[hpdID]
                    xOff = lastGoodAlignment[hpdID][0]
                    yOff = lastGoodAlignment[hpdID][1]
                else:
                    #print " -> No update for HPD", hpdID, "possible. Stick with current DB value"
                    updateOK = False

            # Update the Si alignment with the image movement data
            if updateOK :
                paramName = "dPosXYZ"
                vect = siAlign.paramAsDoubleVect(paramName)
                vect[0] = xOff[0]
                vect[1] = yOff[0]
                vect[2] = 0
                siAlign.addParam( paramName, vect, "" )
                vect = siAlign.paramAsDoubleVect(paramName)
                # Double check update is correct
                if vect[0] != xOff[0] or vect[1] != yOff[0] :
                    print "Warning :  Update mismatch ", xOff[0], yOff[0], ":", vect[0], vect[1]

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
        if type == 'Fill' : os.remove(filename)

        # Update the DB with the HPD alignments for the IOV for this run/fill
        startTime = correctStartTime( unixStartTime )
        stopTime  = cool.ValidityKeyMax
        for xmlpath in alignments.keys():
            # The XML data
            data = alignments[xmlpath]
            # Add the XML footer to the XML data
            data += xmlFooter()
            # First time, create the paths in the DB
            if xmlpath not in createdPaths:
                db.createNode(xmlpath)
                createdPaths += [xmlpath]
            # Add to the DB
            db.storeXMLString( xmlpath, data, startTime, stopTime )

    # Start a PDF file
    globals()["imageFileName"] = "HPDImageCalibrationBy"+type+".ps"
    printCanvas('[')

    # Make plots showing the variations
    for hpd,data in plotData.iteritems():

        #copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)
        #smartID = richSystem().richSmartID(copyNumber)
        #print smartID

        from array import array
        vflag      = array('d')
        vflagerr   = array('d')
        vshiftR    = array('d')
        vshiftRerr = array('d')
        vshiftX    = array('d')
        vshiftXerr = array('d')
        vshiftY    = array('d')
        vshiftYerr = array('d')
        for fl in sorted(data.keys()):
            values = data[fl]
            vflag.append(fl)
            vflagerr.append(0.0)
            vshiftX.append(values['ShiftX'][0])
            vshiftXerr.append(values['ShiftX'][1])
            vshiftY.append(values['ShiftY'][0])
            vshiftYerr.append(values['ShiftY'][0])
            vshiftR.append(values['ShiftR'][0])
            vshiftRerr.append(values['ShiftR'][1])

        plot = TGraphErrors( len(vflag), vflag, vshiftX, vflagerr, vshiftXerr )
        plot.SetTitle( "X Shift HPD Copy Number "+str(hpd) )
        plot.GetXaxis().SetTitle(type)
        plot.GetYaxis().SetTitle("X Offset / mm" )
        plot.Draw("AL*")
        printCanvas()

        plot = TGraphErrors( len(vflag), vflag, vshiftY, vflagerr, vshiftYerr )
        plot.SetTitle( "Y Shift HPD Copy Number "+str(hpd) )
        plot.GetXaxis().SetTitle(type)
        plot.GetYaxis().SetTitle("Y Offset / mm" )
        plot.Draw("AL*")
        printCanvas()

        plot = TGraphErrors( len(vflag), vflag, vshiftR, vflagerr, vshiftRerr )
        plot.SetTitle( "R Shift HPD Copy Number "+str(hpd) )
        plot.GetXaxis().SetTitle(type)
        plot.GetYaxis().SetTitle("sqrt(xOff^2+yOff^2) / mm" )
        plot.Draw("AL*")
        printCanvas()

    # Close the PDF 
    printCanvas(']')

def printCanvas(tag=''):
    canvas = rootCanvas()
    canvas.Update()
    imageType = imageFileName.split(".")[1]
    canvas.Print(imageFileName+tag,imageType)
    # ROOT built in PDFs look crappy. Better to make PS and convert with ps2pdf ...
    if tag == ']' and imageType == 'ps' :
        import os
        print "Converting", imageFileName, "to PDF"
        os.system('ps2pdf '+imageFileName)
        os.remove(imageFileName)
        
def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1000,750)
    return globals()["canvas"]
