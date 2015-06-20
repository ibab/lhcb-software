
# Globals
fitter        = None
imageFileName = ''
canvas        = None
tempDir       = "/var/nwork/pcjj/jonesc/tmp/RichCalib"
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
        from Configurables import DDDBConf, CondDB, LHCbApp, CondDBAccessSvc
        cDB = CondDB()

        #DDDBConf(DataType = "2009")
        #LHCbApp().DDDBtag   = "head-20110303"
        #LHCbApp().CondDBtag = "head-20110524"
        
        #DDDBConf(DataType = "2010")
        #LHCbApp().DDDBtag   = "head-20110303"
        #LHCbApp().CondDBtag = "head-20110524"
        
        #DDDBConf(DataType = "2011")
        #LHCbApp().DDDBtag   = "dddb-20120831"
        #LHCbApp().CondDBtag = "cond-20121208"

        #DDDBConf(DataType = "2012")
        #LHCbApp().DDDBtag   = "dddb-20120831"
        #LHCbApp().CondDBtag = "cond-20121211"

        DDDBConf(DataType = "2015")
        LHCbApp().DDDBtag    = "dddb-20150526"
        LHCbApp().CondDBtag  = "cond-20150601"

        # Move HPD Occs
        #cDB.addLayer(CondDBAccessSvc("NewMDMSCondDB-28022011",
        #                             ConnectionString="sqlite_file:/usera/jonesc/cmtuser/Rec_HEAD/Rich/RichAlignment/scripts/RefractAndHPDJobs/databases/MoveHPDOccs-11052011.db/LHCBCOND",
        #                             DefaultTAG="HEAD"))

        # Set message level to info and above only
        msgSvc().setOutputLevel(3)

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
    iDataSvc()["Structure/LHCb/BeforeMagnetRegion/Rich1"]
    iDataSvc()["Structure/LHCb/AfterMagnetRegion/Rich2"]
    #print iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0"]
    #print iDataSvc()["/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1"]
    #print iDataSvc()["/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PDPanel0"]
    #print iDataSvc()["/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PDPanel1"]
    richSystem()
    
def rootFileListFromTextFile(rootFileList):

    # Open the text file
    files = open(rootFileList,'r')

    # list of root files
    rootfiles = [ ]

    # Read file list from text file
    for file in files.read().split('\n') :
        if len(file) > 0 :
            rootfiles += [file]
    
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

    # Get RichSmartID from copy number
    smartID = richSystem().richSmartID(copyNumber)

    # Path to alignment condition
    alignLoc = "/dd/Conditions/Alignment/"+rich(smartID)+"/"+type+str(copyNumber.data())+"_Align"

    # get the alignment condition
    align = iDataSvc()[alignLoc]

    # Force an update
    #umsSvc().invalidate(align)
    #align.forceUpdateMode()

    # Return the alignment condition
    return align

def siliconAlignmentFilePath(copyNumber):

    # RichSmartID
    smartID = richSystem().richSmartID(copyNumber)
    
    return "/Conditions/"+rich(smartID)+"/Alignment/SiSensors"+panel(smartID)+".xml"

def getHPDmagCond(polarity,copyNumber):

    hpd = getHPD(copyNumber)

    name = ""
    if polarity == "MagDown" : name = "DemagParametersFieldNegative"
    if polarity == "MagUp"   : name = "DemagParametersFieldPositive"

    return hpd.condition(name)

def rich(hpd):
    _rich = ["Rich1","Rich2"]
    return _rich[hpd.rich()]

def panel(hpd):
    return "P"+str(hpd.panel())

def xmlHeader(type,flag):
    return """<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE DDDB SYSTEM "conddb:/DTD/structure.dtd">
<DDDB>

<!-- """+type+""" """+str(flag)+""" -->
"""

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
    # Note dtime must be a date in CET (CERN) time.
    import time
    t = time.mktime(dtime.timetuple())
    zone = time.tzname[0]
    if zone not in ['GMT','CET'] : raise Exception('Unknown time zone '+zone)
    offset = 0
    if zone == 'GMT' : offset = -3600
    return int( (t+offset) * 1e9 )

def correctStartTime(time):
    TimeOffset = 0
    #TimeOffset = int( 5 * 1e9 )
    return time - TimeOffset

def correctStopTime(time):
    TimeOffset = 0
    #TimeOffset = int( 5 * 60 * 1e9 )
    return time + TimeOffset

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

def queryBookKeeping(run):
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    nTries = 0
    res = { 'OK' : False }
    while not res['OK'] and nTries < 100:
        nTries = nTries + 1
        if nTries > 1 :
            print "  -> Problem querying DB -> Will try again after 5 secs ..."
            import time
            time.sleep(5)
        res = BookkeepingClient().getRunInformations(int(run))
    return res

def getAllRuns():
    from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient
    print "Trying to get a list of all runs"
    nTries = 0
    res = { 'OK' : False }
    dict = { 'StartDate'        : "2015-03-01",
             'EndDate'          : "2015-12-31",
             'AllowOutsideRuns' : True, # Allows runs to start inside but finish outside the dates
             'CheckRunStatus'   : False # Don't care if it is processed or not
             }
    while not res['OK'] and nTries < 100:
        nTries = nTries + 1
        if nTries > 1 :
            print "  -> Problem querying DB -> Will try again after 5 secs ..."
            import time
            time.sleep(5)
            res = BookkeepingClient().getRunsForAGivenPeriod(dict)
    runs = [ ]
    if not res['OK']:
        print "ERROR geting list of runs"
    else:
        runs = res['Value']['Runs']
    #print "Runs", runs
    runs.sort()
    return runs

def getNextRun(run,runs):
    i = runs.index(run)
    nextrun = 0
    if i < len(runs)-1 : nextrun = runs[i+1]
    return nextrun
        
def getBestRunEndTime(res,runs):
    rawstop = res['Value']['RunEnd']
    return rawstop

def getRunFillData(rootfiles):

    print "Getting run and fill information ..."

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

        # Load the bookkeeping DB cache
        BookKeepingDBCacheName = "BookKeepingDBCache.pck.bz2"
        bkDBCache = loadDict(BookKeepingDBCacheName)
        
        # Load the Run DB cache for Run info
        RunDBRunCacheName = "RunDBRunCache.pck.bz2"
        runDBRunCache = loadDict(RunDBRunCacheName)

        # Get the list of runs this year
        runs = getAllRuns()

        # Loop over the sorted run list and get the runfilldata
        tmpTime = 0
        for filename in files:
            run = getIntInfo(filename,'Run')
            
            # Get run start and stop times from cache if there
            res = { 'OK' : False }
            if run in bkDBCache.keys() : res = bkDBCache[run]

            if not res['OK'] :

                print " -> Need to query the Bookkeeping DB for run", run, "..."
                res = queryBookKeeping(run)
                if res['OK'] : bkDBCache[run] = res

            rundb_res = { }
            if run in runDBRunCache.keys() :
                rundb_res = runDBRunCache[run]
            else:
                # Access the RUN DB as well
                print " -> Need to query the Run DB for run", run, "...."
                rundb_res = getRunDBRunInfo(int(run))
                runDBRunCache[run] = rundb_res
                
            if res['OK'] :

                # Start and stop times
                start = res['Value']['RunStart']
                stop  = res['Value']['RunEnd']
                #stop  = getBestRunEndTime(res,runs)

                # Run duration in secs
                unixStart = getUNIXTime(start)
                unixStop  = getUNIXTime(stop)
                duration  = (unixStop-unixStart) / (1e9)

                # Check for a long duration, which could suggest the stop time is wrong due to
                # a bug related to the deferred triggering in the Run DB
                if duration > (60*60)+30 :
                    print " -> WARNING : Long duration ... Probably wrong due to RunDB bug ... Will try and fix ..."
                    nextrun = getNextRun(run,runs)
                    print "   -> Old stop time", stop
                    if nextrun != 0 :
                        import datetime
                        print "  -> Looking up info for next run", nextrun
                        nextres = queryBookKeeping(nextrun)
                        if nextres['OK'] :
                            stop = nextres['Value']['RunStart'] - datetime.timedelta(seconds=1)
                        else:
                            print "   -> Problem with Bookkeeping DB lookup. Will assume run lasted one hour"
                            stop = start + datetime.timedelta(seconds=3600)
                    else:
                        print "  -> No next run. Will assume run lasted one hour"
                        import datetime
                        stop = start + datetime.timedelta(seconds=3600)
                    unixStop = getUNIXTime(stop)
                    duration = (unixStop-unixStart) / (1e9)
                    print "   -> New stop time", stop    

                # fill number
                fill = int(res['Value']['FillNumber'])

                # Field polarity ?
                polarity = getFieldPolarity(res)

                # TCK
                tck = res['Value']['Tck']

                # Find the entry for 90000000 (FULL stream)
                tag = -1
                iStream = 0
                for i in res['Value']['Stream'] :
                    if i == 90000000 : tag = iStream
                    iStream += 1

                # Number events in FULL stream
                nEventsPhys = 0
                if tag > 0 : nEventsPhys = res['Value']['Number of events'][tag]

                # Luminosity for FULL stream
                lumi = 0
                if tag > 0 : lumi = res['Value']['luminosity'][tag]

                # Moore Version
                mooreV = "Unknown"
                if "programVersion" in rundb_res.keys() : mooreV = rundb_res["programVersion"]

                # AvInstLumi
                avInLumi = 0
                if "avLumi" in rundb_res.keys() : avInLumi = 1.0e30 * float(rundb_res["avLumi"])

                # Av Mu
                avMu = 0
                if "avMu" in rundb_res.keys() : avMu = float(rundb_res["avMu"])

                # Beta *
                betaStar = 0
                if "betaStar" in rundb_res.keys() : betaStar = float(rundb_res["betaStar"])

                # L0 Rate
                avL0Rate = 0
                if "avL0PhysRate" in rundb_res.keys() : avL0Rate = float(rundb_res["avL0PhysRate"])

                # Hlt Rate
                avHltRate = 0
                if "avHltPhysRate" in rundb_res.keys() : avHltRate = float(rundb_res["avHltPhysRate"])

                # deadtime
                avPhysDeadtime = 0
                if "avPhysDeadTime" in rundb_res.keys() : avPhysDeadtime = float(rundb_res["avPhysDeadTime"])

                # Mag current
                magCurrent = 0
                if "magnetCurrent" in rundb_res.keys() : magCurrent = float(rundb_res["magnetCurrent"])

                # Fill data for this run
                runfilldata["RunData"][run] = { "Start" : start,
                                                "Stop"  : stop,
                                                "Duration" : duration,
                                                "Fill"  : fill,
                                                "FieldPolarity" : polarity,
                                                "PhysEvents" : nEventsPhys,
                                                "Luminosity" : lumi,
                                                "MooreVersion" : mooreV,
                                                "AvInstLuminosity" : avInLumi,
                                                "AvMu"           : avMu,
                                                "BetaStar"       : betaStar,
                                                "AvL0PhysRate"   : avL0Rate,
                                                "AvHLTPhysRate"  : avHltRate,
                                                "AvPhysDeadtime" : avPhysDeadtime,
                                                "MagnetCurrent"  : magCurrent,
                                                "TCK" : tck }
               
                if fill not in runfilldata["FillData"].keys():
                    runfilldata["FillData"][fill] = { "Start" : start,
                                                      "Stop"  : stop,
                                                      "FieldPolarity" : polarity,
                                                      "TCK" : tck,
                                                      "MooreVersion" : mooreV,
                                                      "BetaStar" : betaStar,
                                                      "PhysEvents" : nEventsPhys,
                                                      "Luminosity" : lumi,
                                                      "MagnetCurrent" : magCurrent,
                                                      "Files" : [] }
                fillData = runfilldata["FillData"][fill]
                if fillData["Start"] > start : fillData["Start"] = start
                if fillData["Stop"]  < stop  : fillData["Stop"]  = stop
                fillData["Files"] += [filename]
                print "  -> Run", run, "Fill", fill, polarity, "is from", start, "to", stop
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

        # Print the last time
        print "-> Last date", runfilldata["GlobalStopTime"]

        # Save the result to the cache
        rootFileMD5Cache[md] = runfilldata

        # Pickle the caches
        pickleDict(RootFileNameMD5CacheName,rootFileMD5Cache)
        pickleDict(BookKeepingDBCacheName,bkDBCache)
        pickleDict(RunDBRunCacheName,runDBRunCache)

    # Return the Run Time Information
    return runfilldata

def getRunDBRunInfo(run):
    # Read from the Run DB via the web and convert to python
    import urllib, json
    return json.loads( urllib.urlopen("http://lbrundb.cern.ch/api/run/"+str(run)).read() )

def getRunDBFillInfo(fill):
    # Read from the Run DB via the web and convert to python
    import urllib, json
    return json.loads( urllib.urlopen("http://lbrundb.cern.ch/api/fill/"+str(fill)).read() )

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
    mergedfile = globals()["tempDir"]+"/HPDAlign_Fill-"+str(fill)+"_"+hash+".root"

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
        print "ERROR Getting fill for run", run
        import DIRAC
        DIRAC.exit(1)
    return fill

def runAll(files='2012-RootFiles.txt'):
    hpdImageShiftsFollow(files)
    #hpdImageShiftsAverage(files)
    #hpdOccupancies(files)

def hpdOccupancies(files='2012-RootFiles.txt'):
    calibrationByRuns(rootfiles=files,followType="Smoothed",
                      fitType='Sobel',smoothSigmaHours=0.5,
                      createShiftUpdate=False,createMagUpdate=False,createHPDOccUpdate=True)

def hpdImageShiftsFollow(files='2012-RootFiles.txt'):
    calibrationByRuns(rootfiles=files,followType="Smoothed",
                      fitType='Sobel',smoothSigmaHours=1.0,
                      createShiftUpdate=True,createMagUpdate=False,createHPDOccUpdate=False)

def hpdImageShiftsAverage(files='2012-RootFiles.txt'):
    calibrationByRuns(rootfiles=files,followType="Average",
                      fitType='Sobel',smoothSigmaHours=1.0,
                      createShiftUpdate=True,createMagUpdate=False,createHPDOccUpdate=False)
    
def calibrationByRuns(rootfiles='2012-RootFiles.txt',
                      fitType="Sobel",followType="Smoothed",pol=0,smoothSigmaHours=1,
                      createShiftUpdate=True,createMagUpdate=False,createHPDOccUpdate=True):    
    return calibration(rootfiles,'Run',fitType,followType,pol,smoothSigmaHours,
                       createShiftUpdate,createMagUpdate,createHPDOccUpdate)

def calibrationByFills(rootfiles='2012-RootFiles.txt',
                       fitType="Sobel",followType="Smoothed",pol=0,smoothSigmaHours=12,
                       createShiftUpdate=True,createMagUpdate=False,createHPDOccUpdate=True):    
    return calibration(rootfiles,'Fill',fitType,followType,pol,smoothSigmaHours,
                       createShiftUpdate,createMagUpdate,createHPDOccUpdate)

def calibration(rootfiles,type,fitType,followType,pol,smoothSigmaHours,
                createShiftUpdate,createMagUpdate,createHPDOccUpdate):

    from ROOT import TFile, TGraphErrors, TGraph, TF1, TH2D, TH1D
    import GaudiPython
    from GaudiPython import gbl
    import datetime, time
    from PyCool import cool
    from math import sqrt

    #initialise()

    if followType not in ["Average","FollowMovements","Smoothed"]:
        raise Exception("Unknown Follow Mode "+followType)

    if fitType not in ["Sobel","SimpleChi2","FastRingFit","CppFit"]:
        raise Exception("Unknown Fit Mode "+fitType)

    # Polarities to consider
    polarities = ['MagAll']
    if createMagUpdate : polarities = ['MagDown','MagUp','MagOff']

    # Load the list of root files
    files = rootFileListFromTextFile(rootfiles)

    # Number of HPDs
    minHPDID = 0
    maxHPDID = 484

    # Min number of entries in HPD alignment histogram for update
    minHPDEntries = 10000

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

            # SmartID
            smartID = richSystem().richSmartID(copyNumber)

            # Get the alignment condition
            siAlign = getSiSensorAlignment(copyNumber)

            # Make sure this HPD has an entry
            if hpdID not in plotData.keys() : plotData[hpdID] = { }

            # Default HPD info summary is failed
            plotData[hpdID][flag] = { "FitOK" : False }

            # HPD image fit
            if createShiftUpdate :
                
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
                    # Python based HPD image fits

                    # Get the histogram for this HPD
                    image = file.Get('RICH/RichHPDImageSummary/Rich_HPD_'+str(hpdID)+'_Image')
                    if image != None :
                    
                        if image.GetEntries() >= minHPDEntries :

                            # Get the occ plot, for number of events
                            occplotname = 'RICH/HPDHitsMoni/PDs/NumHits/CopyNum-'+str(hpdID)
                            occplot = file.Get(occplotname)
                            if occplot != None :
                        
                                # Setup the fit object
                                params      = gbl.Rich.HPDImage.HPDFit.Params()
                                params.type = fitType

                                # Do the fit
                                result = hpdfitter().fit(image,params,int(occplot.GetEntries()))

                                # Get the results
                                xOff   = (result.x(),result.xErr())
                                yOff   = (result.y(),result.yErr())
                                rad    = (result.radInMM(),result.radErrInMM())
                                plotData[hpdID][flag] = { "FitOK"   : result.OK(),
                                                          "ShiftR"  : rFromXY(xOff,yOff),
                                                          "ShiftX"  : xOff,
                                                          "ShiftY"  : yOff,
                                                          "Radius"  : rad }
        
            else:

                # No fitting so fake it
                plotData[hpdID][flag] = { "FitOK"   : False,
                                          "ShiftR"  : (0,0),
                                          "ShiftX"  : (0,0),
                                          "ShiftY"  : (0,0),
                                          "Radius"  : (0,0) }                

            # HPD occupancy info
            occ = (0,0)
            ok  = False
            if createHPDOccUpdate :
                #occPlot = file.Get('RICH/HPDL0HitsMoni/PDs/NumHits/CopyNum-'+str(hpdID))
                occPlot = file.Get('RICH/HPDHitsMoni/PDs/NumHits/CopyNum-'+str(hpdID))
                if occPlot :
                    if occPlot.GetEntries() >= 10 :
                        occ = ( occPlot.GetMean(), occPlot.GetMeanError() )
                        ok = True
            plotData[hpdID][flag]["Occupancy"] = occ
            plotData[hpdID][flag]["OccOK"]     = ok

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
    for polarity in polarities : avTrendFit[polarity] = { }
    
    # Smoothers
    smoothers = { }
    for polarity in polarities : smoothers[polarity] = { }
    
    # base name
    basename = rootfiles.split(".")[0]+"-"+type+"Aligned-"+fitType+"-"+followType
    if followType == "Average"   : basename += "Pol"+str(pol)
    if followType == "Smoothed"  : basename += str(smoothSigmaHours)+"hours"
    if createShiftUpdate  : basename += "-HPDAlign"
    if createMagUpdate    : basename += "-HPDDeMag"
    if createHPDOccUpdate : basename += "-HPDOcc"
    basename += "-"+dateString()

    # Set output PDF name
    globals()["imageFileName"] = "results/"+basename+".pdf"
    printCanvas('[')

    # Run range for including in the fitted data
    minMaxFillForFit = [0,9999999]  # All fills/runs

    # Data for HPD occupancy text file
    hpdOccForText = { }

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
        vflag      = {  }
        vflagerr   = {  }
        vshiftR    = {  }
        vshiftRerr = {  }
        vshiftX    = {  }
        vshiftXerr = {  }
        vshiftY    = {  }
        vshiftYerr = {  }
        dbX        = {  }
        dbY        = {  }
        dbR        = {  }
        vTime      = {  }
        vTimeErr   = {  }
        vRadius    = {  }
        vRadErr    = {  }
        vOcc       = {  }
        vOccErr    = {  }
        for polarity in polarities :
            vflag      [polarity] = array('d')
            vflagerr   [polarity] = array('d')
            vshiftR    [polarity] = array('d')
            vshiftRerr [polarity] = array('d')
            vshiftX    [polarity] = array('d')
            vshiftXerr [polarity] = array('d')
            vshiftY    [polarity] = array('d')
            vshiftYerr [polarity] = array('d')
            dbX        [polarity] = array('d')
            dbY        [polarity] = array('d')
            dbR        [polarity] = array('d')
            vTime      [polarity] = array('d')
            vTimeErr   [polarity] = array('d')
            vRadius    [polarity] = array('d')
            vRadErr    [polarity] = array('d')
            vOcc       [polarity] = array('d')
            vOccErr    [polarity] = array('d')

        dataColor   = 1
        splineColor = 2
        refColor    = 4
        smoothColor = 38

        xPlots  = { }
        yPlots  = { }
        rPlots  = { }
        oPlots  = { }
        xPlotsS = { }
        yPlotsS = { }
        rPlotsS = { }
        oPlotsS = { }
        for polarity in polarities:
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
            oPlots[polarity] = TH1D( polarity+"-Occupancy"+str(hpd),
                                     polarity+" Occupancy : Copy Number "+idS, 51, -0.5, 50.5 )
            oPlots[polarity].GetXaxis().SetTitle("Occupancy / # hits")
            oPlots[polarity].SetMarkerColor(dataColor)
            oPlots[polarity].SetLineColor(dataColor)
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
            oPlotsS[polarity] = TH1D( polarity+"-OccupancySmooth"+str(hpd),
                                      polarity+" Occupancy : Copy Number "+idS, 51, -0.5, 50.5 )
            oPlotsS[polarity].GetXaxis().SetTitle("Occupancy / # hits")
            oPlotsS[polarity].SetMarkerColor(smoothColor)
            oPlotsS[polarity].SetLineColor(smoothColor)

        for fl in sorted(data.keys()):

            if fl >= minMaxFillForFit[0] and fl <= minMaxFillForFit[1] :

                # Average time for run/fill
                avTime = ( getUNIXTime(runFillData[type+"Data"][fl]["Start"]) +
                           getUNIXTime(runFillData[type+"Data"][fl]["Stop"]) ) / 2e9

                if avTime < minMaxAvTime[0] : minMaxAvTime[0] = avTime
                if avTime > minMaxAvTime[1] : minMaxAvTime[1] = avTime

                values = data[fl]
                ok = False
                if createShiftUpdate or createMagUpdate : ok = values['FitOK']
                if createHPDOccUpdate : ok = values['OccOK']
                if createHPDOccUpdate and ( createShiftUpdate or createMagUpdate ) :
                    ok = values['FitOK'] and values['OccOK']
                if ok :

                    # Field Polarity
                    polarity = 'MagAll'
                    if createMagUpdate : polarity = runFillData[type+"Data"][fl]["FieldPolarity"]

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
                    vOcc[polarity].append(values['Occupancy'][0])
                    vOccErr[polarity].append(values['Occupancy'][1])

                    # Fill plots
                    xPlots[polarity].Fill( values['ShiftX'][0] )
                    yPlots[polarity].Fill( values['ShiftY'][0] )
                    rPlots[polarity].Fill( values['Radius'][0] )
                    oPlots[polarity].Fill( values['Occupancy'][0] )

                    # text data for occupancies
                    if hpd not in hpdOccForText.keys() : hpdOccForText[hpd] = { }
                    hpdOccForText[hpd][fl] = values['Occupancy'][0]
                        
            else :

                print "Skipping data from", type, fl, "from fit"

        # Loop over polarities
        for polarity in polarities:

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
                if createShiftUpdate : printCanvas() 
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
                if createShiftUpdate : printCanvas()
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
                    val = smootherRad.Eval(t,3600*smoothSigmaHours) 
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
                if createShiftUpdate : printCanvas()
                # ======================================================================================

                # ======================================================================================
                canvas.cd(1)
                plotOcc = TGraphErrors( len(vTime[polarity]), vTime[polarity],
                                        vOcc[polarity], vTimeErr[polarity], vOccErr[polarity] )
                plotOcc.SetTitle( polarity+" HPD Occupancy : Copy Number "+idS )
                plotOcc.GetXaxis().SetTitle(type+" Average time (secs since UNIX epoch)")
                plotOcc.GetYaxis().SetTitle("Occupancy / # hits")
                plotOcc.SetMarkerColor(dataColor)
                plotOcc.SetLineColor(dataColor)
                FitOcc = TF1("AverageHPDOcc"+polarity+idS,"pol"+str(pol),minMaxAvTime[0],minMaxAvTime[1])
                FitOcc.SetParName(0,"Fitted Occupancy / # hits")
                FitOcc.SetLineColor(dataColor)
                plotOcc.Fit(FitOcc,"QRS")
                plotOcc.Draw("ALP")
                labelDataDB(polarity,dataColor,smoothColor)
                smootherOcc = gbl.Rich.HPDImage.GraphSmoother( len(vTime[polarity]),
                                                               vTime[polarity],
                                                               vOcc[polarity],
                                                               vTimeErr[polarity],
                                                               vOccErr[polarity] )
                smoothedOcc = array('d')
                for t in vTime[polarity] :
                    val = smootherOcc.Eval(t,3600*smoothSigmaHours) 
                    smoothedOcc.append( val )
                    oPlotsS[polarity].Fill(val)
                plotSmoothedOcc = TGraph( len(vTime[polarity]), vTime[polarity], smoothedOcc )
                plotSmoothedOcc.SetMarkerColor(smoothColor)
                plotSmoothedOcc.SetLineColor(smoothColor)
                plotSmoothedOcc.Draw("LP")
                canvas.cd(2)
                if oPlots[polarity].GetMaximum() > oPlotsS[polarity].GetMaximum():
                    oPlots[polarity].Draw()
                    oPlotsS[polarity].Draw('SAME')
                else:
                    oPlotsS[polarity].Draw()
                    oPlots[polarity].Draw('SAME')
                if createHPDOccUpdate : printCanvas()

                # ======================================================================================

                # Save fit results
                avTrendFit[polarity][hpd] = [FitX,FitY,FitRad,FitOcc]
                smoothers[polarity][hpd]  = [smootherX,smootherY,smootherRad,smootherOcc]

    # Close the PDF 
    printCanvas(']')

    # Create text files
    if createHPDOccUpdate : createHPDOccTextFile(type,hpdOccForText,basename,runFillData)

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
        polarity = 'MagAll'
        if createMagUpdate : polarity = runFillData[type+"Data"][flag]["FieldPolarity"]
             
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

        # Set UMS to the average time for this run/fill
        iDetDataSvc().setEventTime( gbl.Gaudi.Time((unixStartTime+unixStopTime)/2) )
        umsSvc().newEvent()

        # MDMS conditions
        mdmsConds = { }
        if createMagUpdate :
            for pol in ["MagDown","MagUp"]:
                mdmsConds[pol] = { }
                for r in ["Rich1","Rich2"] :
                    mdmsConds[pol][r] = { }
                    for p in ["P0","P1"] : mdmsConds[pol][r][p] = { }
                for copyN in [0,100,210,351]:
                    hpd = richSystem().richSmartID( gbl.Rich.DAQ.HPDCopyNumber(copyN) )
                    mdmsConds[pol][rich(hpd)][panel(hpd)] = getHPDmagCond(pol,gbl.Rich.DAQ.HPDCopyNumber(copyN))

        # HPD Occupancies
        hpdOccs          = { }
        hpdOccs["Rich1"] = { }
        hpdOccs["Rich2"] = { }

        # Have some HPDs with Occ data for this run/fill
        hpdOccDataOK = False
        
        # Loop over HPDs
        for hpdID in sorted(alignData[flag].keys()):
            values = alignData[flag][hpdID]

            # HPD copy number
            copyNumber = gbl.Rich.DAQ.HPDCopyNumber(hpdID)
            # SmartID
            smartID = richSystem().richSmartID(copyNumber)
            # RICH
            R = rich(smartID)
        
            # Get the alignment condition
            siAlign = getSiSensorAlignment(copyNumber)

            # Get the offsets
            text = basename
            haveupdate = True
            ok = False
            if createShiftUpdate or createMagUpdate : ok = values['FitOK']
            if createHPDOccUpdate : ok = values['OccOK']
            if createHPDOccUpdate and ( createShiftUpdate or createMagUpdate ) :
                ok = values['FitOK'] and values['OccOK']
            if   followType == "FollowMovements" and ok :
                xOff = values["ShiftX"][0]
                yOff = values["ShiftY"][0]
                radF = values["Radius"][0]
                occ  = values["Occupancy"][0]
            elif followType == "Average" and hpdID in avTrendFit[polarity].keys():
                xOff = avTrendFit[polarity][hpdID][0].Eval(avTime)
                yOff = avTrendFit[polarity][hpdID][1].Eval(avTime)
                radF = avTrendFit[polarity][hpdID][2].Eval(avTime)
                occ  = avTrendFit[polarity][hpdID][3].Eval(avTime)
            elif followType == "Smoothed" and hpdID in smoothers[polarity].keys():
                xOff = smoothers[polarity][hpdID][0].Eval(avTime,3600*smoothSigmaHours)
                yOff = smoothers[polarity][hpdID][1].Eval(avTime,3600*smoothSigmaHours)
                radF = smoothers[polarity][hpdID][2].Eval(avTime,3600*smoothSigmaHours)
                occ  = smoothers[polarity][hpdID][3].Eval(avTime,3600*smoothSigmaHours)
            else:
                xOff = values["DBShiftX"]
                yOff = values["DBShiftY"]
                occ  = 0
                text = "From original DB"
                haveupdate = False
 
            # Update the Si alignment with the image movement data
            if createShiftUpdate :
                paramName = "dPosXYZ"
                vect = siAlign.paramAsDoubleVect(paramName)
                vect[0] = xOff
                vect[1] = yOff
                vect[2] = 0
                siAlign.addParam( paramName, vect, "" )
                vect = siAlign.paramAsDoubleVect(paramName)
            
                # The alignment path for the HPD silicon
                alignPath = siliconAlignmentFilePath(copyNumber)
        
                # Get alignment XML file
                if alignPath not in alignments.keys() : alignments[alignPath] = ""
       
                # Add alignments for this HPD
                alignments[alignPath] += hpdXMLComment(copyNumber,text)
                alignments[alignPath] += siAlign.toXml() + '\n'

            # update MDMS image magnification factors for given HPD
            if ( createMagUpdate and haveupdate and polarity != "MagOff" and
                 R == "Rich2" and radF > 0.1 ) :
                pName = "hpd"+str(hpdID)+"_rec"
                cond = mdmsConds[polarity][rich(smartID)][panel(smartID)]
                vect = cond.paramAsDoubleVect(pName)
                vect[1] = magFromRadius(radF)
                cond.addParam( pName, vect, "" )

            # HPD Occ
            hpdOccs[R][smartID.key()] = occ
            if occ > 0 : hpdOccDataOK = True

        # Create HPD.xml updates (RICH2 demag)
        if createMagUpdate :
            for R in ["Rich2"] :
                XmlPath = "/Conditions/"+R+"/Environment/HPD.xml"
                if XmlPath not in alignments.keys() : alignments[XmlPath] = ""
                for pol in ["MagDown","MagUp"]:
                    for P in ["P0","P1"] : 
                        alignments[XmlPath] += mdmsConds[pol][R][P].toXml() + '\n\n'

        # Create HPDOccupancies.xml
        if createHPDOccUpdate :
            if hpdOccDataOK :
                for R in ["Rich1","Rich2"] :
                    XmlPath = "/Conditions/"+R+"/Environment/HPDOccupancies.xml"
                    if XmlPath not in alignments.keys() : alignments[XmlPath] = ""
                    alignments[XmlPath] += xmlHPDOccs(hpdOccs[R])
            else :
                print "  -> HPD Occupancy data missing .... skipped"

        # Update the DB with the HPD alignments for the IOV for this run/fill
        startTime = correctStartTime( unixStartTime )
        stopTime  = cool.ValidityKeyMax
        if followType == "Smoothed" : stopTime = getUNIXTime(runFillData["GlobalStopTime"])
        # End of 2011 Sept TS/MD
        #stopTime = getUNIXTime( datetime.datetime( 2011, 8, 31, 0, 0, 0 ) )
        # End of run
        #stopTime = correctStopTime( unixStopTime )
        # End of 2009
        #stopTime = getUNIXTime( datetime.datetime( 2009, 12, 31, 23, 59, 59 ) )
        # End of 2010
        #stopTime = getUNIXTime( datetime.datetime( 2010, 12, 31, 23, 59, 59 ) )
        # End of 2011
        #stopTime = getUNIXTime( datetime.datetime( 2011, 12, 31, 23, 59, 59 ) )
        # End of Run 126261
        #stopTime = getUNIXTime( datetime.datetime( 2012, 8, 22, 17, 55, 32 ) )
        # End of Run 128492
        #stopTime = getUNIXTime( datetime.datetime( 2012, 9, 17,  7, 0, 0 ) )
        # End of Run 128268 (pA pilot run)
        #stopTime = getUNIXTime( datetime.datetime( 2012, 9, 17,  7, 0, 0 ) )
        # Long way away ...
        #stopTime = getUNIXTime( datetime.datetime( 2100, 12, 31, 23, 59, 59 ) )

        # Loop over XML files in the fitted DB
        if flag > 0 :
            
            for xmlpath in alignments.keys() :
            
                # The XML data
                alignment = alignments[xmlpath]

                # First time, create the paths in the DB
                if xmlpath not in createdPaths:
                    print "  -> Creating DB path", xmlpath 
                    db.createNode(xmlpath)
                    createdPaths += [xmlpath]

                # Check MD5
                mdsum = stringMD5(alignment)
                if xmlpath not in alignMDsums : alignMDsums[xmlpath] = 0

                # If this alignment is different to the last, update
                if mdsum != alignMDsums[xmlpath] :
                    fullalignxml = xmlHeader(type,flag) + alignment + xmlFooter()
                    db.storeXMLString( xmlpath, fullalignxml, startTime, stopTime )
                    alignMDsums[xmlpath] = mdsum
                else:
                    print "  -> Alignment for", xmlpath, "same as previous -> No update"
                    
        else:
            
            print " -> Skipping", type, flag, "from DB slice update"

    print "Done ..."

def createHPDOccTextFile(type,hpdOccForText,basename,runFillData):

    textFileName = "results/"+basename+".txt"
    print "Opening text file", textFileName
    textFile = open(textFileName,'w')

    line = "Run Fill Polarity MooreVersion TCK PhysEvents AvLumi AvMu BetaStar AvL0PhysRate AvHLTPhysRate AvPhysDeadtime MagnetCurrent StartDate StartTime StopDate StopTime Duration "
    hpds = [ ]
    for hpd in hpdOccForText.keys() :
        line += "HPD" + str(hpd) + " "
        hpds += [hpd]
    textFile.write(line+"\n")

    fls = [ ]
    for hpd in hpdOccForText.keys() :
        for fl in hpdOccForText[hpd]:
            if fl not in fls : fls += [fl]

    for fl in sorted(fls) :
        line  = str(fl) + " "
        line += str(runFillData[type+"Data"][fl]["Fill"]) + " "
        line += str(runFillData[type+"Data"][fl]["FieldPolarity"]) + " "
        line += str(runFillData[type+"Data"][fl]["MooreVersion"]) + " "
        line += str(runFillData[type+"Data"][fl]["TCK"]) + " "
        line += str(runFillData[type+"Data"][fl]["PhysEvents"]) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["AvInstLuminosity"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["AvMu"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["BetaStar"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["AvL0PhysRate"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["AvHLTPhysRate"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["AvPhysDeadtime"] ) + " "
        line += str( '%g' % runFillData[type+"Data"][fl]["MagnetCurrent"] ) + " "
        line += str(runFillData[type+"Data"][fl]["Start"]) + " "
        line += str(runFillData[type+"Data"][fl]["Stop"]) + " "
        line += str(runFillData[type+"Data"][fl]["Duration"]) + " "
        for hpd in hpds :
            data = 0
            if hpd in hpdOccForText.keys() :
                if fl in hpdOccForText[hpd].keys():
                    data = hpdOccForText[hpd][fl]
            line += str('%g' % data) + " "
        textFile.write(line+"\n")

    print "Text Summary Done ..."

def magFromRadius(radius):
    if radius == 0 :
        #print "WARNING : radius = 0, using default 6.48"
        return 36.0 / 6.48
    else:
        return 36.0 / radius

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
    splits = imageFileName.split(".")
    nsplits = len(splits)
    if nsplits == 0 :
        print "ERROR : No image type extension on file name", imageFileName
    else:
        imageType = splits[nsplits-1]
        if tag == '[' or tag == '{' : print "Printing to", imageFileName
        canvas.Print(imageFileName+tag,imageType)
        if tag == ']' or tag == '}' : print "Printing done ..."
        
def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1050,750)
    return globals()["canvas"]
        
def xmlHPDOccs(hpdOccs):
    occS = ""
    for hpd in sorted(hpdOccs.keys()) :
        if occS != "" : occS += " "
        occS += str(int32(hpd)) + "/" + str( '%g' % hpdOccs[hpd] )
    return """
<condition classID="5" name="AverageHPDOccupancies">
<paramVector name="Occupancies" type="std::string" comment="Average HPD occupancy">""" + occS + """</paramVector>
</condition>
"""

def int32(x):
    if x>0xFFFFFFFF:
        raise OverflowError
    if x>0x7FFFFFFF:
        x=int(0x100000000-x)
        if x<2147483648:
            return -x
        else:
            return -2147483648
    return x
