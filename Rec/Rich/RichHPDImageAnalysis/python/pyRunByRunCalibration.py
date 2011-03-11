
# Globals
fitter        = None
imageFileName = ''
canvas        = None
tempDir       = "/var/nwork/pciy/jonesc/tmp/HPDImageAlign"
initialised   = False

def initialise():

    if not globals()['initialised'] :
    
        # Check temp dir is valid
        tmpDir = globals()['tempDir']
        import os
        if not os.path.exists(tmpDir):
            os.mkdir(tmpDir)
            if not os.access(tmpDir,os.W_OK):
                raise Exception('Temp Dir '+tmpDir+' Not Writable')

        # Check results dir
        if not os.path.exists("results") : os.mkdir("results")

        from ROOT import gROOT
        # No info messages
        gROOT.ProcessLine("gErrorIgnoreLevel = kWarning;")
        # Batch mode (no TCanvas)
        gROOT.SetBatch(True)

        import GaudiPython
    
        # Initialise a few things
        from Configurables import DDDBConf, CondDB, LHCbApp
        DDDBConf(DataType = "2010")
        LHCbApp().DDDBtag   = "head-20101026"
        LHCbApp().CondDBtag = "head-20101112"
        CondDB()

        # Set message level to warnings and above only
        msgSvc().setOutputLevel(4)

        # Finally, initialize GaudiPython
        GaudiPython.AppMgr().initialize()

        # Initialise various DeRich objects
        loadRichDet()

        # flag as done
        globals()['initialised'] = True

def hpdfitter():
    from GaudiPython import gbl
    if globals()['fitter'] == None :
        globals()['fitter'] = gbl.Rich.HPDImage.HPDFit()
    return globals()['fitter']
        
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

def loadRichDet():
    iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich1"]
    iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel0"]
    iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich2/PDPanel1"]
    richSystem()
    
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

def getHPDAlignment(copyNumber) :
    return getAlignment(copyNumber,"HPD")

def getSiSensorAlignment(copyNumber) :
    return getAlignment(copyNumber,"SiSensor")

def getAlignment(copyNumber,type):

    #print "Getting", type, "alignment"

    from GaudiPython import gbl

    rich = ["Rich1","Rich2"]

    # Get RichSmartID from copy number
    smartID = richSystem().richSmartID(copyNumber)

    # Path to alignment condition
    alignLoc = "/dd/Conditions/Alignment/"+rich[smartID.rich()]+"/"+type+str(copyNumber.data())+"_Align"

    # get the alignment condition
    align = iDataSvc()[alignLoc]

    # Force an update
    #umsSvc().invalidate(align)
    #align.forceUpdateMode()

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
    import pickle, os, bz2
    file = bz2.BZ2File(filename,"w")
    pickle.dump(data,file,2)
    file.close()

def loadDict(filename):
    import pickle, os, bz2
    data = { }
    if os.path.exists(filename) :
        file = bz2.BZ2File(filename,"r")
        data = pickle.load(file)
        file.close()
    return data

def dateString():
    import datetime
    now = datetime.datetime.now()
    return now.strftime("%d%m%Y")

def getRunFillData(rootfiles):

    print "Getting Run and Fill time information"

    # get file MD5 sum
    md = fileMD5(rootfiles)

    RootFileNameMD5CacheName = "RootFileNameMD5Cache.pck.bz2"

    # load cached run/fill info
    rootFileMD5Cache = loadDict(RootFileNameMD5CacheName)

    # Initialise return data
    runfilldata = { }
    runfilldata["RunData"]  = { }
    runfilldata["FillData"] = { }
    runfilldata["GlobalStopTime"] = None

    # Do we already have results for this run/fill
    if md in rootFileMD5Cache.keys() :

        # Yes, so just return the data
        runfilldata = rootFileMD5Cache[md]

    else:

        # No, need to build it
        import datetime, time, os

        # Load the list of root files
        files = rootFileListFromTextFile(rootfiles)

        # Load the raw cached run data
        RunCacheName = "RunInfoCache.pck.bz2"
        runTimeCache = loadDict(RunCacheName)

        # Loop over the sorted run list and get the runfilldata
        tmpTime = 0
        for filename in files:
            run = getIntInfo(filename,'Run')
            
            # Get run start and stop times from cache if there
            res = { 'OK' : False }
            if run in runTimeCache.keys() : res = runTimeCache[run]

            if not res['OK'] :
                from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
                nTries = 0
                while not res['OK'] and nTries < 10:
                    nTries = nTries + 1
                    if nTries > 1 :
                        print " -> Problem querying DB - Will try again after 5 secs ..."
                        import time
                        time.sleep(5)
                    res = BookkeepingClient().getRunInformations(int(run))
                if res['OK'] : runTimeCache[run] = res
                
            if res['OK'] :

                # Start and stop times
                start = res['Value']['RunStart']
                stop  = res['Value']['RunEnd']

                # fill number
                fill  = int(res['Value']['FillNumber'])

                # Field polarity ?
                polarity = getFieldPolarity(res)

                # Fill data for this run
                runfilldata["RunData"][run] = { "Start" : start,
                                                "Stop"  : stop,
                                                "Fill"  : fill,
                                                "FieldPolarity" : polarity }
                
                if fill not in runfilldata["FillData"].keys():
                    runfilldata["FillData"][fill] = { "Start" : start,
                                                      "Stop"  : stop,
                                                      "FieldPolarity" : polarity,
                                                      "Files" : []  }
                fillData = runfilldata["FillData"][fill]
                if fillData["Start"] > start : fillData["Start"] = start
                if fillData["Stop"] < stop   : fillData["Stop"]  = stop
                fillData["Files"] += [filename]
                print " -> Run", run, "Fill", fill, polarity, "is from", start, "to", stop
                unixEndTime = getUNIXTime( stop )
                if unixEndTime > tmpTime :
                    tmpTime = unixEndTime
                    runfilldata["GlobalStopTime"] = stop
                fillData['DataTakingDescription'] = res['Value']['DataTakingDescription']
                
            else:
                
                print "ERROR Getting start/stop times for run", run
                print res
                import DIRAC
                DIRAC.exit(1)

        # Save the result to the cache
        rootFileMD5Cache[md] = runfilldata

        # Pickle the caches
        pickleDict(RootFileNameMD5CacheName,rootFileMD5Cache)
        pickleDict(RunCacheName,runTimeCache)

    # Return the Run Time Information
    return runfilldata

def getFieldPolarity(res):

    desc = res['Value']['DataTakingDescription']
    splits = desc.split("-")

    polarity = ''
    
    for split in splits :
        if split == 'MagDown' or split == 'MagUp' or split == 'MagOff' :
            if polarity == '' :
                polarity = split
            else:
                print "WARNING : DataTaking description has two field polarities !!", desc
                polarity = split

    if polarity == '' :
        print "WARNING : Unable to extract field polarity"
        print res

    return polarity

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

def runAll(files='MDMS-RootFiles.txt'):

    calibrationByRuns(rootfiles=files,followType="Smoothed",fitType='Sobel')
    #calibrationByRuns(rootfiles=files,followType="Smoothed",fitType='SimpleChi2')
    #calibrationByRuns(rootfiles=files,followType="Smoothed",fitType='CppFit')

    #calibrationByRuns(rootfiles=files,followType="FittedPol",fitType='Sobel')
    #calibrationByRuns(rootfiles=files,followType="FittedPol",fitType='SimpleChi2')
    #calibrationByRuns(rootfiles=files,followType="FittedPol",fitType='CppFit')

    #calibrationByFills(rootfiles=files,followType="Smoothed",fitType='Sobel')
    #calibrationByFills(rootfiles=files,followType="Smoothed",fitType='SimpleChi2')
    #calibrationByFills(rootfiles=files,followType="Smoothed",fitType='CppFit')

    #calibrationByFills(rootfiles=files,followType="FittedPol",fitType='Sobel')
    #calibrationByFills(rootfiles=files,followType="FittedPol",fitType='SimpleChi2')
    #calibrationByFills(rootfiles=files,followType="FittedPol",fitType='CppFit')
 
def calibrationByRuns(rootfiles='RootFileNames.txt',
                      fitType="Sobel",followType="Smoothed",pol=0,smoothSigmaHours=3):
    return calibration(rootfiles,'Run',fitType,followType,pol,smoothSigmaHours)

def calibrationByFills(rootfiles='RootFileNames.txt',
                       fitType="Sobel",followType="Smoothed",pol=0,smoothSigmaHours=12):
    return calibration(rootfiles,'Fill',fitType,followType,pol,smoothSigmaHours)

def calibration(rootfiles,type,fitType,followType,pol,smoothSigmaHours):

    from ROOT import TFile, TGraphErrors, TGraph, TF1, TSpline3, TH2D, TH1D
    import GaudiPython
    from GaudiPython import gbl
    import datetime, time
    from PyCool import cool
    from math import sqrt

    initialise()

    if followType not in ["FittedPol","FollowMovements","Smoothed"]:
        raise Exception("Unknown Follow Mode "+followType)

    if fitType not in ["Sobel","SimpleChi2","FastRingFit","CppFit"]:
        raise Exception("Unknown Fit Mode "+fitType)
                       
    # Load the list of root files
    files = rootFileListFromTextFile(rootfiles)

    # Number of HPDs
    minHPDID = 0
    maxHPDID = 484

    # Min number of entries in HPD alignment histogram for update
    minHPDEntries = 500

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
        print "Processing", type , flag, \
              "( #", nFile, "of", len(filesToLoopOver), ")", filename

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

            # Special case for CppFit ....
            if fitType == "CppFit" :
              
                import pyHistoParsingUtils

                # Get the offsets. Use try to catch errors
                try:

                    res = pyHistoParsingUtils.hpdLocalOffset(file,hpdID,minHPDEntries)
                    xOff = res["Result"]["ShiftX"]
                    yOff = res["Result"]["ShiftX"]
                    rad  = res["Result"]["Radius"]
                    plotData[hpdID][flag] = { "FitOK"   : res['OK'],
                                              "ShiftR"  : rFromXY(xOff,yOff),
                                              "ShiftX"  : xOff,
                                              "ShiftY"  : yOff,
                                              "Radius"  : rad }
                    
                except Exception,e:

                    plotData[hpdID][flag] = { "FitOK" : False }

            else:
                # Python based fits

                # Default status failed
                plotData[hpdID][flag] = { "FitOK" : False }

                # Get the histogram for this HPD
                image = file.Get('RICH/RichHPDImageSummary/Rich_HPD_'+str(hpdID)+'_Image')
                if image != None :
                    
                    if image.GetEntries() >= minHPDEntries :
                        
                        # Setup the fit object
                        params      = gbl.Rich.HPDImage.HPDFit.Params()
                        params.type = fitType
                        
                        # Do the fit
                        result = hpdfitter().fit(image,params)
                        
                        # Get the results
                        xOff   = (result.x(),result.xErr())
                        yOff   = (result.y(),result.yErr())
                        rad    = (result.radInMM(),result.radErrInMM())
                        plotData[hpdID][flag] = { "FitOK"   : result.OK(),
                                                  "ShiftR"  : rFromXY(xOff,yOff),
                                                  "ShiftX"  : xOff,
                                                  "ShiftY"  : yOff,
                                                  "Radius"  : rad }

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

    # Fitters
    avTrendFit = { }
    avTrendFit['MagDown'] = { }
    avTrendFit['MagUp']   = { }
    avTrendFit['MagOff']  = { }
    # Smoothers
    smoothers = { }
    smoothers['MagDown'] = { }
    smoothers['MagUp']   = { }
    smoothers['MagOff']  = { }
    
    # base name
    basename = rootfiles.split(".")[0]+"-"+type+"Aligned-"+fitType+"-"+followType
    if followType == "FittedPol" : basename += str(pol)
    if followType == "Smoothed"  : basename += str(smoothSigmaHours)+"hours"
    basename += "-"+dateString()

    # Set output PDF name
    globals()["imageFileName"] = "results/"+basename+".pdf"
    printCanvas('[')

    # Run range for including in the fitted data
    minMaxFillForFit = [0,9999999]  # All fills/runs
    #minMaxFillForFit = [952, 1430] # Range used by Matt for Mirror alignment 28/10/2010

    for hpd,data in plotData.iteritems():

        # HPD copy number
        copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpd)
        # HPD SmartID
        smartID = richSystem().richSmartID(copyNumber)
        # HPD ID string
        idS = str(hpd) + " " + smartID.toString()

        # Min max values
        #minMaxFlag   = [1e9,-1e9]
        minMaxAvTime = [1e9,-1e9]

        from array import array
        vflag      = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vflagerr   = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftR    = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftRerr = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftX    = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftXerr = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftY    = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vshiftYerr = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        dbX        = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        dbY        = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        dbR        = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vTime      = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vTimeErr   = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vRadius    = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }
        vRadErr    = { 'MagDown' : array('d'), 'MagUp' : array('d'), 'MagOff' : array('d') }

        dataColor   = 1
        splineColor = 2
        refColor    = 4
        smoothColor = 38

        xPlots  = { }
        yPlots  = { }
        rPlots  = { }
        xPlotsS = { }
        yPlotsS = { }
        rPlotsS = { }
        for polarity in ['MagDown','MagUp','MagOff']:
            xPlots[polarity] = TH1D( polarity+"-XShifts"+str(hpd),
                                     polarity+" X-Shifts : Copy Number "+idS, 100, -1.0, 1.0 )
            xPlots[polarity].GetXaxis().SetTitle("X Shifts / mm")
            xPlots[polarity].SetMarkerColor(dataColor)
            xPlots[polarity].SetLineColor(dataColor)
            yPlots[polarity] = TH1D( polarity+"-YShifts"+str(hpd),
                                     polarity+" Y-Shifts : Copy Number "+idS, 100, -1.0, 1.0 )
            yPlots[polarity].GetXaxis().SetTitle("Y Shifts / mm")
            yPlots[polarity].SetMarkerColor(dataColor)
            yPlots[polarity].SetLineColor(dataColor)
            rPlots[polarity] = TH1D( polarity+"-Radius"+str(hpd),
                                     polarity+" Image Radius : Copy Number "+idS, 100, 6.0, 7.0 )
            rPlots[polarity].GetXaxis().SetTitle("Image Radius / mm")
            rPlots[polarity].SetMarkerColor(dataColor)
            rPlots[polarity].SetLineColor(dataColor)
            xPlotsS[polarity] = TH1D( polarity+"-XShiftsSmooth"+str(hpd),
                                     polarity+" Smoothed X-Shifts : Copy Number "+idS, 100, -1.0, 1.0 )
            xPlotsS[polarity].GetXaxis().SetTitle("X Shifts / mm")
            xPlotsS[polarity].SetMarkerColor(smoothColor)
            xPlotsS[polarity].SetLineColor(smoothColor)
            yPlotsS[polarity] = TH1D( polarity+"-YShiftsSmooth"+str(hpd),
                                     polarity+" Smoothed Y-Shifts : Copy Number "+idS, 100, -1.0, 1.0 )
            yPlotsS[polarity].GetXaxis().SetTitle("Y Shifts / mm")
            yPlotsS[polarity].SetMarkerColor(smoothColor)
            yPlotsS[polarity].SetLineColor(smoothColor)
            rPlotsS[polarity] = TH1D( polarity+"-RadiusSmooth"+str(hpd),
                                     polarity+" Smoothed Image Radius : Copy Number "+idS, 100, 6.0, 7.0 )
            rPlotsS[polarity].GetXaxis().SetTitle("Image Radius / mm")
            rPlotsS[polarity].SetMarkerColor(smoothColor)
            rPlotsS[polarity].SetLineColor(smoothColor)

        for fl in sorted(data.keys()):

            if fl >= minMaxFillForFit[0] and fl <= minMaxFillForFit[1] :

                #if fl < minMaxFlag[0] : minMaxFlag[0] = fl
                #if fl > minMaxFlag[1] : minMaxFlag[1] = fl

                # Average time for run/fill
                avTime = ( getUNIXTime(runFillData[type+"Data"][fl]["Start"]) +
                           getUNIXTime(runFillData[type+"Data"][fl]["Stop"]) ) / 2e9

                if avTime < minMaxAvTime[0] : minMaxAvTime[0] = avTime
                if avTime > minMaxAvTime[1] : minMaxAvTime[1] = avTime

                values = data[fl]
                if values['FitOK'] :

                    # Field Polarity
                    polarity = runFillData[type+"Data"][fl]["FieldPolarity"]

                    # Fill arrays
                    vflag[polarity].append(fl)
                    vflagerr[polarity].append(0.0)
                    vshiftX[polarity].append(values['ShiftX'][0])
                    vshiftXerr[polarity].append(values['ShiftX'][1])
                    vshiftY[polarity].append(values['ShiftY'][0])
                    vshiftYerr[polarity].append(values['ShiftY'][1])
                    vshiftR[polarity].append(values['ShiftR'][0])
                    vshiftRerr[polarity].append(values['ShiftR'][1])
                    dbX[polarity].append(values["DBShiftX"])
                    dbY[polarity].append(values["DBShiftY"])
                    dbR[polarity].append(rFromXY([values["DBShiftX"],0],[values["DBShiftY"],0])[0])
                    vTime[polarity].append(avTime)
                    vTimeErr[polarity].append(0.0)
                    vRadius[polarity].append(values['Radius'][0])
                    vRadErr[polarity].append(values['Radius'][1])

                    # Fill plots
                    xPlots[polarity].Fill( values['ShiftX'][0] )
                    yPlots[polarity].Fill( values['ShiftY'][0] )
                    rPlots[polarity].Fill( values['Radius'][0] )
                        
            else :

                print "Skipping data from", type, fl, "from fit"

        # Loop over polarities
        for polarity in ['MagDown','MagUp','MagOff']:

            # If we have data, fill it properly
            if len(vTime[polarity]) > 0:

                canvas = rootCanvas()
                canvas.Clear()
                canvas.Divide(1,2)
                
                # ======================================================================================
                canvas.cd(1)
                plotX = TGraphErrors( len(vTime[polarity]), vTime[polarity],
                                      vshiftX[polarity], vTimeErr[polarity], vshiftXerr[polarity] )
                plotX.SetTitle( polarity+" X Shift : Copy Number "+idS )
                plotX.GetXaxis().SetTitle(type+" Average time (secs since UNIX epoch)")
                plotX.GetYaxis().SetTitle("X Offset / mm" )
                plotX.SetMarkerColor(dataColor)
                plotX.SetLineColor(dataColor)
                FitX = TF1("AverageFitX"+polarity+idS,"pol"+str(pol),minMaxAvTime[0],minMaxAvTime[1])
                FitX.SetParName(0,"Fitted Shift / mm")
                FitX.SetLineColor(dataColor)
                plotX.Fit(FitX,"QRS")
                plotX.Draw("ALP")
                labelDataDB(polarity,dataColor,smoothColor)
                smootherX = gbl.Rich.HPDImage.GraphSmoother( len(vTime[polarity]),
                                                             vTime[polarity],
                                                             vshiftX[polarity],
                                                             vTimeErr[polarity],
                                                             vshiftXerr[polarity] )
                smoothedX = array('d')
                for t in vTime[polarity] :
                    val = smootherX.Eval(t,3600*smoothSigmaHours)
                    smoothedX.append( val )
                    xPlotsS[polarity].Fill(val)
                plotSmoothedX = TGraph( len(vTime[polarity]), vTime[polarity], smoothedX )
                plotSmoothedX.SetMarkerColor(smoothColor)
                plotSmoothedX.SetLineColor(smoothColor)
                plotSmoothedX.Draw("LP")
                canvas.cd(2)
                if xPlots[polarity].GetMaximum() > xPlotsS[polarity].GetMaximum():
                    xPlots[polarity].Draw()
                    xPlotsS[polarity].Draw('SAME')
                else:
                    xPlotsS[polarity].Draw()
                    xPlots[polarity].Draw('SAME')
                printCanvas() 
                # ======================================================================================

                # ======================================================================================
                canvas.cd(1)
                plotY = TGraphErrors( len(vTime[polarity]), vTime[polarity],
                                      vshiftY[polarity], vTimeErr[polarity], vshiftYerr[polarity] )
                plotY.SetTitle( polarity+" Y Shift : Copy Number "+idS )
                plotY.GetXaxis().SetTitle(type+" Average time (secs since UNIX epoch)")
                plotY.GetYaxis().SetTitle("Y Offset / mm" )
                plotY.SetMarkerColor(dataColor)
                plotY.SetLineColor(dataColor)
                FitY = TF1("AverageFitX"+polarity+idS,"pol"+str(pol),minMaxAvTime[0],minMaxAvTime[1])
                FitY.SetParName(0,"Fitted Shift / mm")
                FitY.SetLineColor(dataColor)
                plotY.Fit(FitY,"QRS")
                plotY.Draw("ALP")
                labelDataDB(polarity,dataColor,smoothColor)
                smootherY = gbl.Rich.HPDImage.GraphSmoother( len(vTime[polarity]),
                                                             vTime[polarity],
                                                             vshiftY[polarity],
                                                             vTimeErr[polarity],
                                                             vshiftYerr[polarity] )
                smoothedY = array('d')
                for t in vTime[polarity] :
                    val = smootherY.Eval(t,3600*smoothSigmaHours) 
                    smoothedY.append( val )
                    yPlotsS[polarity].Fill(val)
                plotSmoothedY = TGraph( len(vTime[polarity]), vTime[polarity], smoothedY )
                plotSmoothedY.SetMarkerColor(smoothColor)
                plotSmoothedY.SetLineColor(smoothColor)
                plotSmoothedY.Draw("LP")
                canvas.cd(2)
                if yPlots[polarity].GetMaximum() > yPlotsS[polarity].GetMaximum():
                    yPlots[polarity].Draw()
                    yPlotsS[polarity].Draw('SAME')
                else:
                    yPlotsS[polarity].Draw()
                    yPlots[polarity].Draw('SAME')
                printCanvas()
                # ======================================================================================

                # ======================================================================================
                canvas.cd(1)
                plotRad = TGraphErrors( len(vTime[polarity]), vTime[polarity],
                                        vRadius[polarity], vTimeErr[polarity], vRadErr[polarity] )
                plotRad.SetTitle( polarity+" Image Radius : Copy Number "+idS )
                plotRad.GetXaxis().SetTitle(type+" Average time (secs since UNIX epoch)")
                plotRad.GetYaxis().SetTitle("Image Radius / mm")
                plotRad.SetMarkerColor(dataColor)
                plotRad.SetLineColor(dataColor)
                FitRad = TF1("AverageFitRadius"+polarity+idS,"pol"+str(pol),minMaxAvTime[0],minMaxAvTime[1])
                FitRad.SetParName(0,"Fitted Radius / mm")
                FitRad.SetLineColor(dataColor)
                plotRad.Fit(FitRad,"QRS")
                plotRad.Draw("ALP")
                labelDataDB(polarity,dataColor,smoothColor)
                smootherRad = gbl.Rich.HPDImage.GraphSmoother( len(vTime[polarity]),
                                                               vTime[polarity],
                                                               vRadius[polarity],
                                                               vTimeErr[polarity],
                                                               vRadErr[polarity] )
                smoothedRad = array('d')
                for t in vTime[polarity] :
                    val =smootherRad.Eval(t,3600*smoothSigmaHours) 
                    smoothedRad.append( val )
                    rPlotsS[polarity].Fill(val)
                plotSmoothedRad = TGraph( len(vTime[polarity]), vTime[polarity], smoothedRad )
                plotSmoothedRad.SetMarkerColor(smoothColor)
                plotSmoothedRad.SetLineColor(smoothColor)
                plotSmoothedRad.Draw("LP")
                canvas.cd(2)
                if rPlots[polarity].GetMaximum() > rPlotsS[polarity].GetMaximum():
                    rPlots[polarity].Draw()
                    rPlotsS[polarity].Draw('SAME')
                else:
                    rPlotsS[polarity].Draw()
                    rPlots[polarity].Draw('SAME')
                printCanvas()
                # ======================================================================================

                # Save fit results
                avTrendFit[polarity][hpd] = [FitX,FitY]
                smoothers[polarity][hpd]  = [smootherX,smootherY]

    # Close the PDF 
    printCanvas(']')

    # Resort the data as run as primary key
    alignData = { }
    for hpd,data in plotData.iteritems() :
        for flag,values in data.iteritems() :
            if flag not in alignData.keys() : alignData[flag] = { }
            alignData[flag][hpd] = values

    # Open new alignment SQL slice(s)
    dbName = "results/"+basename+".db"
    db = createDBFile(dbName)

    # List of paths already created in the DB
    createdPaths = [ ]

    # Save MD5 sums for each alignment string
    alignMDsums = { }

    # Finally, loop over the data to create DB updates per HPD
    nflag = 0
    for flag in sorted(alignData.keys()):
        nflag += 1

        # Get field polarity
        polarity = runFillData[type+"Data"][flag]["FieldPolarity"]
                
        print " -> Creating alignment update for", type, flag, polarity, \
              "( #", nflag, "of", len(alignData.keys()), ")"

        # alignments for this run/fill
        alignments = { }

        # Get run/fill start and stop time
        startTime = runFillData[type+"Data"][flag]["Start"]
        stopTime  = runFillData[type+"Data"][flag]["Stop"]
        unixStartTime = getUNIXTime(startTime)
        unixStopTime  = getUNIXTime(stopTime)

        # Average time for run/fill in secs
        avTime = ( unixStartTime + unixStopTime ) / 2e9

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
            text = basename
            if   followType == "FollowMovements" and values["FitOK"]:
                xOff = values["ShiftX"][0]
                yOff = values["ShiftY"][0]
            elif followType == "FittedPol" and hpdID in avTrendFit[polarity].keys():
                xOff = avTrendFit[polarity][hpdID][0].Eval(avTime)
                yOff = avTrendFit[polarity][hpdID][1].Eval(avTime)
            elif followType == "Smoothed" and hpdID in smoothers[polarity].keys():
                xOff = smoothers[polarity][hpdID][0].Eval(avTime,3600*smoothSigmaHours)
                yOff = smoothers[polarity][hpdID][1].Eval(avTime,3600*smoothSigmaHours)
            else:
                xOff = values["DBShiftX"]
                yOff = values["DBShiftY"]
                text = "From original DB"
 
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

def labelDataDB(polarity,alignColor,smoothColor):
    from ROOT import TText
    text = TText()
    text.SetNDC()
    text.SetTextSize(0.02)
    text.SetTextColor(alignColor)
    text.DrawText( 0.13, 0.85, polarity+" Fitted Image Shifts" )
    text.SetTextColor(smoothColor)
    text.DrawText( 0.13, 0.82, "Smoothed" )
    
def printCanvas(tag=''):
    canvas = rootCanvas()
    canvas.Update()
    imageType = imageFileName.split(".")[1]
    canvas.Print(imageFileName+tag,imageType)
    if tag == '[' or tag == '{' :
        print "Printing to", globals()["imageFileName"]
    elif tag == ']' or tag == '}' :
        if imageType == "ps" :
            import os
            imageBase = imageFileName.split(".")[0]
            print " -> Converting to PDF"
            os.system('convert '+imageFileName+" "+imageBase+".pdf")
            os.remove(imageFileName)
        print "Printing done ..."
        
def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1050,750)
    return globals()["canvas"]
