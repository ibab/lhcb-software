
# Globals
imageFileName = ''
canvas        = None
tempDir       = "/var/work/jonesc/tmp/HPDImageAlign"

def initialise():

    # Check temp dir is valid
    tmpDir = globals()['tempDir']
    import os
    if not os.path.exists(tmpDir):
        os.mkdir(tmpDir)
    if not os.access(tmpDir,os.W_OK):
        raise Exception('Temp Dir'+tmpDir+'Not Writable')

    from ROOT import gROOT
    # No info messages
    gROOT.ProcessLine("gErrorIgnoreLevel = kWarning;")
    # Batch mode (no TCanvas)
    gROOT.SetBatch(True)
    
    # Initialise a few things
    from Configurables import DDDBConf, CondDB, LHCbApp
    DDDBConf(DataType = "2010")
    LHCbApp().DDDBtag   = "head-20100518"
    LHCbApp().CondDBtag = "head-20100730"
    CondDB()

    # Set message level to warnings and above only
    msgSvc().setOutputLevel(4)

    # Finally, initialize GaudiPython
    import GaudiPython
    GaudiPython.AppMgr().initialize()

def msgSvc():
    import GaudiPython
    return GaudiPython.AppMgr().service('MessageSvc','IMessageSvc')

def iDataSvc():
    import GaudiPython
    return GaudiPython.AppMgr().detSvc()

def umsSvc():
    import GaudiPython
    GaudiPython.AppMgr().createSvc('UpdateManagerSvc')
    return GaudiPython.AppMgr().service('UpdateManagerSvc','IUpdateManagerSvc')

def iDetDataSvc():
    import GaudiPython
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
"""
#<!-- HPD Image alignment for """ + str(type) + """ """ + str(flag) + """ -->

def xmlFooter():
    return """
</DDDB>"""

def hpdXMLComment(copyNumber,text=""):
    smartID = richSystem().richSmartID(copyNumber)
    hardID  = richSystem().hardwareID(smartID)
    return """
<!-- HPD hardwareID H"""+str(hardID.data())+"""  Copy# """+str(copyNumber.data())+""" """+text+"""-->
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

def stringsMD5(strings):
    string = ''
    for str in strings : string += str
    return stringMD5(string)

def stringMD5(string):
    import hashlib
    m = hashlib.md5()
    m.update(string)
    return m.hexdigest()

def fileMD5(file):
    # Compute the MD5 sum of the root files list
    import hashlib
    infile = open(file,'rb')
    content = infile.read()
    infile.close()
    m = hashlib.md5()
    m.update(content)
    return m.hexdigest()

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

def getRunFillData(rootfiles):

    print "Getting Run and Fill time information"

    # get file MD5 sum
    md = fileMD5(rootfiles)

    RunFillCacheName = "RunFillInfoCache.pck"
    RunCacheName     = "RunInfoCache.pck"

    # load cached run/fill info
    runFillTimeCache = loadDict(RunFillCacheName)

    # Initialise return data
    runfilldata = { }
    runfilldata["RunData"] = { }
    runfilldata["FillData"] = { }
    runfilldata["GlobalStopTime"] = None

    # Do we already have results for this run/fill
    if md in runFillTimeCache.keys() :

        # Yes, so just return the data
        runfilldata = runFillTimeCache[md]

    else:

        # No, need to build it
        import datetime, time, os

        # Load the list of root files
        files = rootFileListFromTextFile(rootfiles)

        # Load the raw cached run data
        runTimeCache = loadDict(RunCacheName)

        # Loop over the sorted run list and get the runfilldata
        tmpTime = 0
        for filename in files:
            run = getIntInfo(filename,'Run')
            # Get run start and stop times
            if run in runTimeCache.keys():
                res = runTimeCache[run]
            else:
                from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
                res = BookkeepingClient().getRunInformations(int(run))
                runTimeCache[run] = res
            if res['OK'] :
                start = res['Value']['RunStart']
                stop  = res['Value']['RunEnd']
                fill  = int(res['Value']['FillNumber'])
                runfilldata["RunData"][run] = { "Start" : start, "Stop" : stop, "Fill" : fill }
                if fill not in runfilldata["FillData"].keys():
                    runfilldata["FillData"][fill] = { "Start" : start,
                                                      "Stop"  : stop,
                                                      "Files" : []  }
                fillData = runfilldata["FillData"][fill]
                if fillData["Start"] > start : fillData["Start"] = start
                if fillData["Stop"] < stop   : fillData["Stop"]  = stop
                fillData["Files"] += [filename]
                print " -> Run", run, "Fill", fill, "is from", start, "to", stop
                unixEndTime = getUNIXTime( stop )
                if unixEndTime > tmpTime :
                    tmpTime = unixEndTime
                    runfilldata["GlobalStopTime"] = stop
            else:
                print "ERROR Getting start/stop times for run", run
                import DIRAC
                DIRAC.exit(1)

        # Save the result to the cache
        runFillTimeCache[md] = runfilldata

        # Pickle the caches
        pickleDict(RunFillCacheName,runFillTimeCache)
        pickleDict(RunCacheName,runTimeCache)

    # Return the Run Time Information
    return runfilldata

def rFromXY(a,b):

    from math import sqrt

    RR = a[0]*a[0]+b[0]*b[0]
    R = 0
    if RR > 0 : R = sqrt(RR)

    tempA = a[0]*a[0]*a[1]*a[1] + b[0]*b[0]*b[1]*b[1]
    tempB = a[0]*a[0] + b[0]*b[0]
    error = 0
    if tempB > 0 : error = sqrt( tempA / tempB )

    return (R,error)

def mergeRootFile(fill,infiles):

    import os

    # Get unique MD5 hash for input files
    hash = stringsMD5(infiles)

    # merged ROOT file for given fill data
    mergedfile = globals()["tempDir"]+"/HPDAlign_Fill-"+str(fill)+"_"+stringsMD5(infiles)+".root"

    # Make if not there
    if not os.path.exists(mergedfile) : 
        command = "hadd "+mergedfile
        for infile in infiles : command += " " + str(infile)
        print command
        os.system(command)

    return mergedfile

def runToFill(run):
    # Database API
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    res = BookkeepingClient().getRunInformations(int(run))
    fill = 0
    if res['OK'] :
        fill = res['Value']['FillNumber']
    else:
        print "ERROR Getting Fill for run", run
        import DIRAC
        DIRAC.exit(1)
    return fill

def calibrationByRuns(rootfiles='RootFileNames.txt',
                      fullFit=False,forceAverages=False):
    return calibration(rootfiles,'Run',fullFit,forceAverages)

def calibrationByFills(rootfiles='RootFileNames.txt',
                       fullFit=False,forceAverages=False):
    return calibration(rootfiles,'Fill',fullFit,forceAverages)

def calibration(rootfiles,type,fullFit,forceAverages):

    import pyHistoParsingUtils
    from ROOT import TFile, TGraphErrors, TGraph, TF1
    import GaudiPython
    from GaudiPython import gbl
    import datetime, time
    from PyCool import cool
    from math import sqrt

    fitType = "CppFit"
    if fullFit : fitType = "FullFit"
    avType = "Full"
    if forceAverages : avType = "Average"
        
    # Load the list of root files
    files = rootFileListFromTextFile(rootfiles)

    # Number of HPDs
    minHPDID = 0
    maxHPDID = 484
    #minHPDID = 9
    #maxHPDID = 10

    # Min number of entries in HPD alignment histogram for update
    minHPDEntries = 1000

    # Get the run/fill info
    runFillData = getRunFillData(rootfiles)

    # If calibrating by Fill, group and merge files
    filesToLoopOver = files
    if type == 'Fill' :
        filesToLoopOver = [ ]
        for fill in sorted(runFillData["FillData"].keys()):
            infiles = runFillData["FillData"][fill]["Files"]
            filesToLoopOver += [mergeRootFile(fill,infiles)]

    # Save shift data for plots
    plotData = { }

    # Loop over the root files and fit the HPD images
    nFile = 0
    for filename in filesToLoopOver:

        # Open the root file
        file = TFile(filename)

        # Run/fill number
        flag = getIntInfo(filename,type)

        # Get run/fill start and stop time
        startTime = runFillData[type+"Data"][flag]["Start"]
        stopTime  = runFillData[type+"Data"][flag]["Stop"]
        unixStartTime = getUNIXTime(startTime)
        unixStopTime  = getUNIXTime(stopTime)

        nFile += 1
        print "Processing", type , flag, "( #", nFile, "of", len(filesToLoopOver), ")", filename

        # Set UMS to the start time for this run/fill
        iDetDataSvc().setEventTime( gbl.Gaudi.Time(unixStartTime) )
        umsSvc().newEvent()
        
        # Loop over all HPD copy IDs
        for hpdID in range(minHPDID,maxHPDID):

            # HPD copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)

            # Get the alignment condition
            siAlign = getSiSensorAlignment(copyNumber)

            # Make sure this HPD has an entry
            if hpdID not in plotData.keys() : plotData[hpdID] = { }
               
            # Get the offsets. Use try to catch errors
            try:

                offsets = pyHistoParsingUtils.hpdLocalOffset(file,hpdID,minHPDEntries,fullFit)
                xOff    = offsets[0]
                yOff    = offsets[1]
                plotData[hpdID][flag] = { "FitOK"   : True,
                                          "ShiftR"  : rFromXY(xOff,yOff),
                                          "ShiftX"  : xOff,
                                          "ShiftY"  : yOff }
                
            except Exception,e:

                plotData[hpdID][flag] = { "FitOK" : False }

            # Save the DB values for plotting
            dbShift = siAlign.paramAsDoubleVect("dPosXYZ")
            if dbShift.size() == 3:
                plotData[hpdID][flag]["DBShiftX"] = dbShift[0]
                plotData[hpdID][flag]["DBShiftY"] = dbShift[1]
            else:
                plotData[hpdID][flag]["DBShiftX"] = 0
                plotData[hpdID][flag]["DBShiftY"] = 0

            # Add the start/stop times for this run/fill
            plotData[hpdID][flag]["UnixStartTime"] = unixStartTime
            plotData[hpdID][flag]["UnixStopTime"]  = unixStopTime

        # close the ROOT file
        file.Close()

    # Average X and Y values
    averageShifts = { }

    # Make plots showing the variations
    print "Making summary plots"

    fileType = ".pdf"

    # Start a PDF file
    globals()["imageFileName"] = "HPDAlignBy"+type+"-"+fitType+avType+fileType
    printCanvas('[')

    for hpd,data in plotData.iteritems():

        # Min max flag values
        minMaxFlag = [999,-999]

        from array import array
        vflag      = array('d')
        vflagerr   = array('d')
        vshiftR    = array('d')
        vshiftRerr = array('d')
        vshiftX    = array('d')
        vshiftXerr = array('d')
        vshiftY    = array('d')
        vshiftYerr = array('d')
        dbX        = array('d')
        dbY        = array('d')
        dbR        = array('d')
        
        for fl in sorted(data.keys()):
            
            if fl < minMaxFlag[0] : minMaxFlag[0] = fl
            if fl > minMaxFlag[1] : minMaxFlag[1] = fl
            
            values = data[fl]
            if values['FitOK'] :
                vflag.append(fl)
                vflagerr.append(0.0)
                vshiftX.append(values['ShiftX'][0])
                vshiftXerr.append(values['ShiftX'][1])
                vshiftY.append(values['ShiftY'][0])
                vshiftYerr.append(values['ShiftY'][1])
                vshiftR.append(values['ShiftR'][0])
                vshiftRerr.append(values['ShiftR'][1])
                dbX.append(values["DBShiftX"])
                dbY.append(values["DBShiftY"])
                dbR.append(rFromXY([values["DBShiftX"],0],[values["DBShiftY"],0])[0])

        if len(vflag) > 0:

            alignColor = 1
            refColor   = 4

            linearFit = TF1("AverageFit","pol0",minMaxFlag[0],minMaxFlag[1])
            linearFit.SetParName(0,"Fitted Shift")
            
            plotX = TGraphErrors( len(vflag), vflag, vshiftX, vflagerr, vshiftXerr )
            plotX.SetTitle( "X Shift HPD Copy Number "+str(hpd) )
            plotX.GetXaxis().SetTitle(type+" Number")
            plotX.GetYaxis().SetTitle("X Offset / mm" )
            plotX.SetMarkerColor(alignColor)
            plotX.SetLineColor(alignColor)
            plotX.Fit(linearFit,"QRS")
            avX = linearFit.GetParameter(0)
            plotX.Draw("ALP")
            plotXDB = TGraph( len(vflag), vflag, dbX )
            plotXDB.SetMarkerColor(refColor)
            plotXDB.SetLineColor(refColor)
            plotXDB.Draw("LP")
            labelDataDB(alignColor,refColor)
            printCanvas()
            
            plotY = TGraphErrors( len(vflag), vflag, vshiftY, vflagerr, vshiftYerr )
            plotY.SetTitle( "Y Shift HPD Copy Number "+str(hpd) )
            plotY.GetXaxis().SetTitle(type+" Number")
            plotY.GetYaxis().SetTitle("Y Offset / mm" )
            plotY.SetMarkerColor(alignColor)
            plotY.SetLineColor(alignColor)
            plotY.Fit(linearFit,"QRS")
            avY = linearFit.GetParameter(0)
            plotY.Draw("ALP")
            plotYDB = TGraph( len(vflag), vflag, dbY )
            plotYDB.SetMarkerColor(refColor)
            plotYDB.SetLineColor(refColor)
            plotYDB.Draw("LP")
            labelDataDB(alignColor,refColor)
            printCanvas()
            
            # Save the fitted averages for this HPD
            averageShifts[hpd] = [avX,avY]

            plotR = TGraphErrors( len(vflag), vflag, vshiftR, vflagerr, vshiftRerr )
            plotR.SetTitle( "R Shift HPD Copy Number "+str(hpd) )
            plotR.GetXaxis().SetTitle(type+" Number")
            plotR.GetYaxis().SetTitle("sqrt(xOff^2+yOff^2) / mm" )
            plotR.SetMarkerColor(alignColor)
            plotR.SetLineColor(alignColor)
            plotR.Fit(linearFit,"QRS")
            plotR.Draw("ALP")
            plotRDB = TGraph( len(vflag), vflag, dbR )
            plotRDB.SetMarkerColor(refColor)
            plotRDB.SetLineColor(refColor)
            plotRDB.Draw("LP")
            labelDataDB(alignColor,refColor)
            printCanvas()

    # Close the PDF 
    printCanvas(']')

    # Resort the data as run as primary key
    alignData = { }
    for hpd,data in plotData.iteritems() :
        for flag,values in data.iteritems() :
            if flag not in alignData.keys() : alignData[flag] = { }
            alignData[flag][hpd] = values

    # Open new alignment SQL slice(s)
    dbName = "HPDAlignBy"+type+"-"+fitType+avType+".db"
    db = createDBFile(dbName)

    # List of paths already created in the DB
    createdPaths = [ ]

    # Save MD5 sums for each alignment string
    alignMDsums = { }

    # Finally, loop over the data to create DB updates per HPD
    nflag = 0
    for flag in sorted(alignData.keys()):
        nflag += 1
        print " -> Creating alignment update for", type, flag, \
              "( #", nflag, "of", len(alignData.keys()), ")"

        # alignments for this run/fill
        alignments = { }

        # Get run/fill start and stop time
        startTime = runFillData[type+"Data"][flag]["Start"]
        stopTime  = runFillData[type+"Data"][flag]["Stop"]
        unixStartTime = getUNIXTime(startTime)
        unixStopTime  = getUNIXTime(stopTime)

        # Set UMS to the start time for this run/fill
        iDetDataSvc().setEventTime( gbl.Gaudi.Time(unixStartTime) )
        umsSvc().newEvent()

        # Loop over HPDs
        for hpdID in sorted(alignData[flag].keys()):
            values = alignData[flag][hpdID]

            # HPD copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)
        
            # Get the alignment condition
            siAlign = getSiSensorAlignment(copyNumber)

            # Get the offsets
            if values["FitOK"] and not forceAverages :
                xOff = values["ShiftX"][0]
                yOff = values["ShiftY"][0]
                text = "From Fit"
            else:
                if hpdID in averageShifts.keys():
                    xOff = averageShifts[hpdID][0]
                    yOff = averageShifts[hpdID][1]
                    text = "From Average"
                    #print "Using average for", type, flag, "HPD", hpdID, "Offsets", xOff, yOff
                else:
                    xOff = values["DBShiftX"]
                    yOff = values["DBShiftY"]
                    text = "From original DB"
                    #print "Using DB for", type, flag, "HPD", hpdID, "Offsets", xOff, yOff

            # Update the Si alignment with the image movement data
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
            
            # The alignment path for the HPD silicon
            alignPath = siliconAlignmentFilePath(copyNumber)
        
            # Get alignment XML file
            if alignPath not in alignments.keys() : alignments[alignPath] = xmlHeader(type,flag)
        
            # Add alignments for this HPD
            alignments[alignPath] += hpdXMLComment(copyNumber,text)
            alignments[alignPath] += siAlign.toXml()  + '\n'

        # Update the DB with the HPD alignments for the IOV for this run/fill
        startTime = correctStartTime( unixStartTime )
        stopTime  = cool.ValidityKeyMax

        # Loop over XML files in the fitted DB
        for xmlpath in alignments.keys() :
            
            # The XML data
            alignment = alignments[xmlpath]
            # Add the XML footer to the XML data
            alignment += xmlFooter()

            # First time, create the paths in the DB
            if xmlpath not in createdPaths:
                db.createNode(xmlpath)
                createdPaths += [xmlpath]

            # Check MD5
            mdsum = stringMD5(alignment)
            if xmlpath not in alignMDsums : alignMDsums[xmlpath] = 0

            # If this alignment is different to the last, update
            if mdsum != alignMDsums[xmlpath] :
                db.storeXMLString( xmlpath, alignment, startTime, stopTime )
                alignMDsums[xmlpath] = mdsum
            else:
                print "  -> Alignment for", xmlpath, "same as previous -> No update"
            
    print "Done ..."

def createDBFile(name):
    import CondDBUI
    import os
    print "Creating DB Slice", name
    if os.path.exists(name) : os.remove(name)
    return CondDBUI.CondDB( "sqlite_file:"+name+"/LHCBCOND",
                            create_new_db=True, readOnly=False )

def labelDataDB(dataColor,dbColor):
    from ROOT import TText
    text = TText()
    text.SetNDC()
    text.SetTextSize(0.03)
    text.SetTextColor(dataColor)
    text.DrawText( 0.13, 0.85, "Fitted Image Shifts" )
    text.SetTextColor(dbColor)
    text.DrawText( 0.13, 0.81, "LHCbCond" )
    
def printCanvas(tag=''):
    canvas = rootCanvas()
    canvas.Update()
    imageType = imageFileName.split(".")[1]
    canvas.Print(imageFileName+tag,imageType)
    # ROOT built in PDFs look crappy. Better to make PS and convert with ps2pdf ...
    #if tag == ']' and imageType == 'ps' :
    #    import os
    #    os.system('ps2pdf '+imageFileName)
    #    os.remove(imageFileName)
        
def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1000,750)
    return globals()["canvas"]
