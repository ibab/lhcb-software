
# Globals
imageFileName = ''
canvas        = None
runInfoCacheName = "RunInfoCache.pck"
runInfoCache  = { }

# ====================================================================================
# Main Methods
# ====================================================================================

## Submits Control Jobs
def submitControlJobs(name="",pickedRuns="Run71813-LFNs.pck"):
    
    import os
    from Ganga.GPI import ( Job, LHCbDataset, Brunel, File, DiracSplitter, 
                            SmartMerger, Dirac )
    
    # Number of target events to process
    nEventsTotal = 250000
    nFilesMax    = 10000

    RunLFNs = getRunLFNData(pickedRuns)

    # Loop over runs
    for run in sorted(RunLFNs.keys()):

        # LFNs for this run
        lfns = RunLFNs[run]
        if len(lfns)>0 :

            nFiles = len(lfns)
            if nFiles > nFilesMax : nFiles = nFilesMax
            nEventsPerFile = nEventsTotal / nFiles
            print "Using", nFiles, "data file(s),", nEventsPerFile, "events per file"

            r = drange( 0.98000, 1.010, 0.001 )
            R1N = [ '%g' % x for x in r ]
    
            r = drange( 1.055, 1.085, 0.001 )
            R2N = [ '%g' % x for x in r ]
        
            for i in range(0,len(R1N)):

                r1 = R1N[i]
                r2 = R2N[i]

                print "(n-1) Scale Rich1 =",r1,"Rich2",r2
            
                # Make a job object
                j = Job( application = Brunel( version = 'v37r7' ) )

                # name
                j.name = "RefInControl"
                if name != "" : j.name += "-"+name
                j.name += "_BR-"+BrunelVer
                j.name += "_Run-"+str(run)+"_R1-"+r1+"_R2-"+r2
                print "Submitting Job", j.name

                # Custom options for this job
                tmpOptsFile = createTempOptsFile(j.name)
                extraopts = open(tmpOptsFile,"w")
                extraopts.write("from Configurables import Brunel\n")
                extraopts.write("from Brunel.Configuration import *\n")
                extraopts.write("from Configurables import UpdateManagerSvc\n")
                extraopts.write("ums = UpdateManagerSvc()\n")
                extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich1/RefractivityScaleFactor := double CurrentScaleFactor = "+r1+";\"]\n")
                extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich2/RefractivityScaleFactor := double CurrentScaleFactor = "+r2+";\"]\n")
                extraopts.write("HistogramPersistencySvc().OutputFile = \""+j.name+".root\"\n")
                extraopts.write("Brunel().EvtMax = "+str(nEventsPerFile)+"\n")
                extraopts.close()

                # Job options
                j.application.optsfile = [ File('CKRefractCalib-FullBrunelReco.py'),
                                           File(tmpOptsFile) ]

                # Set the LFNs to run over
                j.inputdata = LHCbDataset(lfns)

                # Split job into 1 file per subjob
                j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = nFiles )

                # Merge the output
                j.merger = SmartMerger( files = [j.name+".root"],
                                        ignorefailed = True, overwrite = True )

                # Optional input files
                j.inputsandbox = []

                # Dirac backend
                j.backend = Dirac()

                # Submit !!
                j.submit()

## Submits DB calibration jobs
def submitCalibrationJobs(name="",BrunelVer="v37r7",pickedRunsList=[]):
    submitRecoJobs(name,BrunelVer,pickedRunsList,"RefInCalib")

## Submit DB Verification Jobs
def submitVerificationJobs(name="",BrunelVer="v37r7",pickedRunsList=[]):
    submitRecoJobs(name,BrunelVer,pickedRunsList,"RefInVerify")

## Real underlying method
def submitRecoJobs(name,BrunelVer,pickedRunsList,jobType):

    from Ganga.GPI import ( Job, LHCbDataset, Brunel, File,
                            DiracSplitter, SmartMerger, Dirac )

    # If pickled run data list is empty, create full list
    if len(pickedRunsList) == 0 :
        import glob
        pickedRunsList = glob.glob('RunData/*.pck')
    print " Submitting jobs for RunData", pickedRunsList

    # Number of target events to process
    if jobType == "RefInCalib" :
        nEventsTotal = 500000
        nFilesMax    = 150
    else:
        nEventsTotal = 250000
        nFilesMax    = 100

    # Loop over the list of pickled run data files
    for pickedRuns in pickedRunsList:

        # Loop over runs
        print "Reading runs from", pickedRuns
        RunLFNs = getRunLFNData(pickedRuns)
        sortedRuns = sorted(RunLFNs.keys())
        
        print "Submitting", jobType, "jobs for runs", sortedRuns
        nJob = 0
        for run in sortedRuns:

            # Count jobs
            nJob += 1

            # LFNs for this run
            lfns = RunLFNs[run]
            if len(lfns)>0 :

                # Construct the job name
                jobname = jobType
                if name != "" : jobname += "-"+name
                jobname += "_BR-"+BrunelVer + "_Run-"+str(run)

                # is this job already submitted ?
                if jobExists(jobname):

                    print "Job", jobname, "already submitted"

                else:

                    # Configure number of files and events per file
                    nFiles = len(lfns)
                    if nFiles > nFilesMax : nFiles = nFilesMax
                    nEventsPerFile = nEventsTotal / nFiles
          
                    # Make a job object
                    j = Job( application = Brunel( version = BrunelVer ), name = jobname )

                    # Custom options for this job
                    tmpOptsFile = createTempOptsFile(j.name)
                    extraopts = open(tmpOptsFile,"w")

                    # Sandbox
                    mySandBox = [ ]

                    # Basic additions
                    extraopts.write("from Configurables import Brunel\n")
                    extraopts.write("from Brunel.Configuration import *\n")
                    extraopts.write("HistogramPersistencySvc().OutputFile = \""+j.name+".root\"\n")
                    extraopts.write("Brunel().EvtMax = "+str(nEventsPerFile)+"\n")

                    # Only for Calibration jobs
                    if jobType == "RefInCalib" :
                        extraopts.write("from Configurables import UpdateManagerSvc\n")
                        extraopts.write("ums = UpdateManagerSvc()\n")
                        extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich1/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n")
                        extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich2/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n")
                        
                    # For verification jobs, use custom DB Slice(s)
                    if jobType == "RefInVerify" :
                        extraopts.write("from Gaudi.Configuration import *\n")
                        extraopts.write("from Configurables import CondDB, CondDBAccessSvc, LHCbApp\n")
                        # Main DB
                        mainLHCbCond = "LHCBCOND_NewRichAlign_head20100730.db"
                        extraopts.write("CondDB().PartitionConnectionString[\"LHCBCOND\"] = \"sqlite_file:"+mainLHCbCond+"/LHCBCOND\"\n")
                        extraopts.write("LHCbApp().CondDBtag = \"HEAD\"\n")
                        mySandBox += ["databases/"+mainLHCbCond]
                        # Additional DB Slices
                        dbFiles = ["NewRichCKRefIndexCalib"]
                        #dbFiles = [ "TrackAlign-v4.0.VeloYFixed",
                        #            "NewRichCKRefIndexCalib",
                        #            "NewFittedHPDAlignmentsByFill",
                        #            "NewAverageHPDAlignmentsByFill",
                        #            "HPDAlignByFill-FullFitAverage",
                        #            "Rich1MirrorAlignment-v1",
                        #            "Rich2MirrorAlignment-v1" ]
                        for dbFile in dbFiles:
                            extraopts.write("CondDB().addLayer(CondDBAccessSvc(\""+dbFile+"\",ConnectionString=\"sqlite_file:"+dbFile+".db/LHCBCOND\",DefaultTAG=\"HEAD\"))\n")
                            mySandBox += ["databases/"+dbFile+".db"]

                    # Close file
                    extraopts.close()

                    # Job options
                    j.application.optsfile = [ File('CKRefractCalib-FullBrunelReco.py'),
                                               File(extraopts.name) ]

                    # Set the LFNs to run over
                    if nFiles == len(lfns):
                        j.inputdata = LHCbDataset(lfns)
                    else:
                        import random
                        j.inputdata = LHCbDataset( random.sample(lfns,nFiles) )

                    # Split job into 1 file per subjob
                    j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = nFiles )

                    # Merge the output
                    j.merger = SmartMerger( files = [j.name+".root"],
                                            ignorefailed = True, overwrite = True )

                    # Optional input files
                    j.inputsandbox = mySandBox

                    # Dirac backend
                    j.backend = Dirac()
                    # maxCPUlimit = 50000
                    # j.backend = Dirac( settings = {'CPUTime':maxCPUlimit} )

                    # Force jobs to go to CERN only
                    j.backend.settings['Destination'] = 'LCG.CERN.ch'

                    # Submit !!
                    print "Submitting Job", j.name, "( #", nJob, "of", len(sortedRuns), ")"
                    print " -> Using", nFiles, "data files,", nEventsPerFile, "events per file"
                    j.submit()

def refractiveIndexCalib(jobs,rad='Rich1Gas'):

    from Ganga.GPI import Job
    from ROOT import TGraphErrors, TH1F, TF1
    from array import array
    import pickle

    if len(jobs) == 0 : return

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RefIndexCalib"+getJobCaliName(jobs[0])+".pdf"
    printCanvas('[')

    # Dictionary to store the calibration data
    calibrations = { }

    # Keep tabs on min and max values (for plots)
    minMaxScale = [999.0,-999.0]
    if 'Rich1Gas' == rad :
        minMaxCKRes = (0.0013,0.0028)
    else:
        minMaxCKRes = (0.00065,0.0009)

    # Raw mean and sigma
    ckmeans  = { }
    cksigmas = { }

    # Loop over jobs
    nFailedFits = 0
    print "Looping over the runs ..."
    for j in jobs :

        run = int(getInfoFromJob(j,'Run'))

        #print "Fitting job", rad, "Run =", run
        fitResult = fitCKThetaHistogram(j,rad)

        if fitResult['OK'] :
            scale = nScaleFromShift(fitResult,rad)
            if scale[0] < minMaxScale[0] : minMaxScale[0] = scale[0]
            if scale[0] > minMaxScale[1] : minMaxScale[1] = scale[0]
            calibrations[run] = scale
            ckmeans[run]  = fitResult['Mean']
            cksigmas[run] = fitResult['Sigma']
        else:
            nFailedFits += 1
            print "WARNING : Fit failed for run", run, "-", fitResult['Message']

    # Write out calibrations to a pickled python file
    file = open(rad+"-RefIndexCalib.pck","w")
    pickle.dump(calibrations,file)
    file.close()

    # 1D Plot of scale factors
    scaleHist = TH1F( "scaleFactors", rad+" (n-1) Scale Factors",
                      100, 0.999*minMaxScale[0], 1.0001*minMaxScale[1] )

    # 1D Plot of Fitted CK resolutions
    ckResHist = TH1F( "ckRes", rad+" Delta CK Theta Resolution",
                      100, 0.99*minMaxCKRes[0], 1.001*minMaxCKRes[1] )
  
    # For plots (manually make sure sorted by run)
    runs      = array('d')
    runsErr   = array('d')
    scales    = array('d')
    scalesErr = array('d')
    means     = array('d')
    meansErr  = array('d')
    sigmas    = array('d')
    sigmasErr = array('d')
    for run in sorted(calibrations.keys()):
        scale   = calibrations[run]
        ckmean  = ckmeans[run]
        cksigma = cksigmas[run]
        runs.append(float(run))
        runsErr.append(0.0)
        scales.append(scale[0])
        scalesErr.append(scale[1])
        means.append(ckmean[0])
        meansErr.append(ckmean[1])
        sigmas.append(cksigma[0])
        sigmasErr.append(cksigma[1])
        # Fill 1D histo(s)
        scaleHist.Fill(scale[0])
        ckResHist.Fill(cksigma[0])

    # Make the plots
    if len(runs) > 0 :

        linearFit = TF1("AverageFit","pol0",runs[0],runs[len(runs)-1])
        linearFit.SetParName(0,"Mean")
            
        meanTrend = TGraphErrors( len(runs),runs,means,runsErr,meansErr )
        meanTrend.SetTitle( rad+" <Delta CK Theta> by Run" )
        meanTrend.GetXaxis().SetTitle("LHCb Run Number")
        meanTrend.GetYaxis().SetTitle("<Delta CK Theta> / mrad")
        meanTrend.Draw("ALP")
        printCanvas()

        ckFitFunc = TF1("CKRes"+rad,"gaus",minMaxCKRes[0],minMaxCKRes[1])
        ckResHist.Fit(ckFitFunc,"QR")
        ckResHist.Draw('E')
        printCanvas()

        sigmaTrend = TGraphErrors( len(runs),runs,sigmas,runsErr,sigmasErr )
        sigmaTrend.SetTitle( rad+" Delta CK Theta Resolution by Run" )
        sigmaTrend.GetXaxis().SetTitle("LHCb Run Number")
        sigmaTrend.GetYaxis().SetTitle("Delta CK Theta Resolution / mrad")
        sigmaTrend.Fit(linearFit,"QRS")
        sigmaTrend.Draw("ALP")
        printCanvas()

        fitFunc = TF1("Scale"+rad,"gaus",minMaxScale[0],minMaxScale[1])
        scaleHist.Fit(fitFunc,"QR")
        scaleHist.Draw('E')
        printCanvas()

        scaleTrend = TGraphErrors( len(runs),runs,scales,runsErr,scalesErr )
        scaleTrend.SetTitle( rad+" (n-1) corrections by Run" )
        scaleTrend.GetXaxis().SetTitle("LHCb Run Number")
        scaleTrend.GetYaxis().SetTitle("(n-1) Scale Factor")
        #scaleTrend.Fit(linearFit,"QRS")
        scaleTrend.Draw("ALP")
        printCanvas()
       
    # Close PDF file
    printCanvas(']')

    if nFailedFits > 0 :
        print "WARNING :", nFailedFits, " histogram fits failed"
    
def refractiveIndexControl(jobs,rad='Rich1Gas'):
    
    from Ganga.GPI import Job
    from ROOT import TGraphErrors
    from array import array

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RefIndexControl.pdf"
    printCanvas('[')

    x  = array('d')
    xe = array('d')
    y  = array('d')
    ye = array('d')

    indexname = "R1"
    if rad == "Rich2Gas" : indexname = "R2"

    for j in jobs :

        run      = int(getInfoFromJob(j,'Run'))
        refIndex = getInfoFromJob(j,indexname)
  
        print "Fitting job", rad, "Run =", run, "index =", refIndex
        fitResult = fitCKThetaHistogram(j,rad)

        if fitResult['OK'] :
            x.append(float(refIndex))
            xe.append(0.0)
            y.append(float(fitResult['Mean'][0]))
            ye.append(float(fitResult['Mean'][1]))
        else:
            print " -> Fit failed -", fitResult['Message']

    graph = TGraphErrors( len(x),x,y,xe,ye )
    graph.SetTitle( rad+" (n-1) Control plot" )
    graph.GetXaxis().SetTitle("(n-1) Scale factor")
    graph.GetYaxis().SetTitle("CK theta shift")
    graph.Draw("ALP")
    printCanvas()
    
    maxIndex = len(x) - 1
    deltax = x[maxIndex] - x[0]
    deltay = y[0] - y[maxIndex]

    slope = deltax/deltay
    print "(n-1) correction =", slope, "* observed peak shift"
    
    # Close PDF file
    printCanvas(']')

def expectedCKTheta(jobs,rad='Rich1Gas'):

    from Ganga.GPI import Job
    from ROOT import TGraphErrors, TH1
    from array import array

    # Parameters
    minEntries = 10000

    globals()["imageFileName"] = rad+"-ExpectCKtheta.pdf"
    printCanvas('[')

    runs      = array('d')
    runsErr   = array('d')
    exp       = array('d')
    expErr    = array('d')

    for j in jobs :

        # Run number
        run = int(getInfoFromJob(j,'Run'))

        # Load the root file for this job
        rootfile = getRootFile(j)
        if rootfile != None :

            # Get the histogram
            histName = 'RICH/RiCKResLong/'+rad+'/thetaExpect'
            hist = rootfile.Get(histName)

            # Basic check on the histograms before fitting
            entries = hist.GetEntries()
            if entries > minEntries :
        
                hist.Draw()

                # Add Run number to page
                addRunToPlot(run)

                # Print
                printCanvas()
        
                # Mean of the histo
                mean = hist.GetMean()
                err  = hist.GetMeanError()

                runs.append(float(run))
                runsErr.append(0.0)
                exp.append(mean)
                expErr.append(err)
        
    # Make a plot
    graph = TGraphErrors( len(runs),runs,exp,runsErr,expErr )
    graph.SetTitle( rad+" <Expected CK theta> by Run" )
    graph.GetXaxis().SetTitle("Run Number")
    graph.GetYaxis().SetTitle("<Expected CK theta>")
    graph.Draw("ALP")
    printCanvas()
       
    # Close output file
    printCanvas(']')

def recoCKTheta(jobs,rad='Rich1Gas'):

    from Ganga.GPI import Job
    from ROOT import TGraphErrors
    from array import array
    import pickle

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RecoCKtheta.pdf"
    printCanvas('[')

    runs      = array('d')
    runsErr   = array('d')
    reco      = array('d')
    recoErr   = array('d')

    for j in jobs :

        run = int(getInfoFromJob(j,'Run'))

        print "Fitting job", rad, "Run =", run
        fitResult = fitCKThetaHistogram(j,rad,'thetaRec')

        if fitResult['OK'] :
            runs.append(float(run))
            runsErr.append(0.0)
            reco.append(float(fitResult['Mean'][0]))
            recoErr.append(float(fitResult['Mean'][1]))
        else:
            print " -> Fit failed -", fitResult['Message']
            
    # Make a plot
    graph = TGraphErrors( len(runs),runs,reco,runsErr,recoErr )
    graph.SetTitle( rad+" Peak Reco CK Theta by Run" )
    graph.GetXaxis().SetTitle("Run Number")
    graph.GetYaxis().SetTitle("CK Theta Peak / mrad")
    graph.Draw("ALP")
    printCanvas()
       
    # Close PDF file
    printCanvas(']')

# ====================================================================================
# Utility Methods
# ====================================================================================

def jobExists(jobname):
    from Ganga.GPI import jobs
    slice = jobs.select(name=jobname)
    return len(slice) > 0

def getInfoFromJob(j,info='Run'):
    run = 0
    split = ''
    splits = j.name.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == info : run = s[1]
    return run

def getJobCaliName(j):
    tmpA = j.name.split('_')[0]
    tmpB = tmpA.split('-')
    cName = ""
    if len(tmpB) == 2 : cName = tmpB[1]
    return cName

def loadDict(filename):
    import pickle, os
    data = { }
    if os.path.exists(filename) :
        file = open(filename,"r")
        data = pickle.load(file)
        file.close()
    return data

def pickleDict(filename,data):
    import pickle, os
    file = open(filename,"w")
    pickle.dump(data,file)
    file.close()

def loadRunInfoCache():
    cachename = globals()["runInfoCacheName"]
    print "Loading Run Info cache -", cachename
    globals()["runInfoCache"] = loadDict(cachename)

def saveRunInfoCache():
    cachename = globals()["runInfoCacheName"]
    print "Saving Run Info cache  -", cachename
    pickleDict(cachename,globals()["runInfoCache"])

def getRunInformation(run):

    # cached info
    runInfoCache = globals()["runInfoCache"]

    # If cache has entry for this run use that. Otherwise fill.
    if run in runInfoCache.keys():
        res = runInfoCache[run]
    else:
        print "Getting information for run", run, "from BK API... Be patient..."
        from Ganga.GPI import diracAPI
        cmd = ( "from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient;" +
                "result = BookkeepingClient().getRunInformations("+str(run)+")" )
        res = diracAPI(cmd)
        if len(res.keys()) > 0 and res.has_key('OK'):
            if res['OK']:
                runInfoCache[run] = res

    # return the result
    info = { }
    if res['OK'] : info = res['Value']
    return info
                        
def getListOfJobs(tag,name,BrunelVer,statuscodes,MinRun=0,MaxRun=99999999,desc=""):
    from Ganga.GPI import Job, jobs
    cacheLoaded = False
    cJobs = [ ]
    dict = { }
    searchString = tag
    if name != "" : searchString += "-"+name
    for j in jobs :
        if j.status in statuscodes :
            namesplit = j.name.split('_')
            if namesplit[0] == searchString :
                if BrunelVer == '' or namesplit[1] == "BR-"+BrunelVer :
                    run = int(getInfoFromJob(j,'Run'))
                    if run >= MinRun and run <= MaxRun:
                        if desc == "":
                            dict[run] = j
                        else:
                            if not cacheLoaded:
                                loadRunInfoCache()
                                cacheLoaded = True
                            runInfo = getRunInformation(run)
                            if runInfo['DataTakingDescription'] == desc:
                                dict[run] = j
    for d in sorted(dict.keys()) : cJobs += [dict[d]]
    if cacheLoaded : saveRunInfoCache()
    return cJobs

def getCalibrationJobList(name="",BrunelVer="v37r7",statuscodes=['completed'],
                          MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInCalib',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def getVerificationJobList(name="",BrunelVer="v37r7",statuscodes=['completed'],
                           MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInVerify',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def getControlJobList(name="",BrunelVer="v37r7",statuscodes=['completed'],
                      MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInControl',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def nScaleFromShift(shift,rad='Rich1Gas'):
    # As of RICH S/W meeting 3/9/2010
    #slope = 38.2388535346
    #if rad == 'Rich2Gas': slope = 68.2
    # Test values
    slope = 38.24
    if rad == 'Rich2Gas': slope = 68.19
    # Compute the scale factor and its error
    result = 1.0 + (shift['Mean'][0]*slope)
    error  = shift['Mean'][1]*slope
    # Return the values
    return [result,error]

def getRootFilePath(j):
    listofMerged = [ j.outputdir+f for f in j.merger.files ]
    filename = ""
    if len(listofMerged) > 0 : filename = listofMerged[0]
    return filename

def getListOfRootFiles(cjobs):
    names = [ ]
    for j in cjobs:
        name = getRootFilePath(j)
        if name != "" :
            names += [name]
    return names

def dumpRootFileNamesToText(cjobs,filename='RootFileNames.txt'):
    names = getListOfRootFiles(cjobs)
    file = open(filename,'w')
    #file.writelines(names)
    for name in names : file.write(name+"\n")
    #for name in names : file.writeline(name)
    file.close()
    
def getRootFile(j):
    import os
    from ROOT import TFile
    file = None
    filename = getRootFilePath(j)
    if filename != "" :
        if os.path.exists(filename):
            file = TFile( filename )
        else:
            print "ERROR :", filename, "does not exist"
    else:
        print "ERROR Accessing ROOT file for job", j.id
    return file

def removeSubJobOutputData(js=[]):
    from Ganga.GPI import jobs
    import os
    import shutil
    if js == [] : js = jobs
    for j in js:
        if j.status == "completed":
            rootFile = getRootFile(j)
            if rootFile != None:
                rootFile.Close()
                print "Removing subjob output data for job", j.id
                for subj in j.subjobs:
                    if os.path.exists(subj.outputdir):
                        shutil.rmtree(subj.outputdir)
            else:
                print "WARNING : Job", j.id, "completed but root file cannot be opened"
        else:
            print "Job", j.id, "is not completed. Nothing removed"

def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        import RootStyle
        RootStyle.rootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1000,750)
    return globals()["canvas"]

def printCanvas(tag=''):
    canvas = rootCanvas()
    imageType = imageFileName.split(".")[1]
    if tag == "[" : print "Opening file", imageFileName
    if tag != "[" and tag != "]" : canvas.Update()
    canvas.Print(imageFileName+tag,imageType)
    # ROOT built in PDFs look crappy. Better to make PS and convert with ps2pdf ...
    if tag == ']' and imageType == 'ps' :
        import os
        print "Converting", imageFileName, "to PDF"
        os.system('ps2pdf '+imageFileName)
        os.remove(imageFileName)  
        
def fitCKThetaHistogram(j,rad='Rich1Gas',plot='ckResAll'):

    from ROOT import TF1, TH1, TText
    #from ROOT import TFitResultPtr, TFitResult

    # Parameters
    minEntries = 75000

    # Default return result
    result = { 'OK' : False, "Message" : "No Message" }

    # Run number
    run = getInfoFromJob(j,'Run')

    # Load the root file for this job
    rootfile = getRootFile(j)
    if rootfile == None :

        result['Message'] = "Failed to open ROOT file"

    else:

        # Get the histogram
        histName = 'RICH/RiCKResLong/'+rad+'/'+plot
        hist = rootfile.Get(histName)

        # Basic check on the histograms before fitting
        entries = hist.GetEntries()
        if entries < minEntries :

            result['Message'] = "Too few histogram entries"

        else:

            preFitColor  = 12
            fullFitColor = 2
            bkgColor     = 4

            # Get x value of highest content bin
            # (rough estimate of peak position)
            xPeak = hist.GetBinCenter(hist.GetMaximumBin())

            # Pre Fitting range
            delta = 0.0025
            if rad == 'Rich2Gas' : delta = 0.00105
            fitMin = xPeak - delta
            fitMax = xPeak + delta

            # Gaussian function
            preFitFType = "gaus"
            preFitF = TF1(rad+"PreFitF",preFitFType,fitMin,fitMax)
            preFitF.SetLineColor(preFitColor)
 
            # Do the pre fit with just a Gaussian
            hist.Fit(preFitF,"QRS0")

            # Full Fitting range
            delta = 0.007
            if rad == 'Rich2Gas' : delta = 0.004
            fitMin = preFitF.GetParameter(1) - delta
            fitMax = preFitF.GetParameter(1) + delta
            # Absolute max/min values
            if  rad == 'Rich1Gas' :
                if fitMax >  0.0052 : fitMax =  0.0058
                if fitMin < -0.0052 : fitMin = -0.0058
            else:
                if fitMax >  0.0025 : fitMax =  0.0025
                if fitMin < -0.0029 : fitMin = -0.0029

            # First Gaus + pol1
            fbkgFuncType = "pol1"
            fFuncType = "gaus(0)+"+fbkgFuncType+"(3)"
            fFitF = TF1(rad+"FFitF",fFuncType,fitMin,fitMax)
            fFitF.SetParName  (0,"Gaus Constant")
            fFitF.SetParName  (1,"Gaus Mean")
            fFitF.SetParName  (2,"Gaus Sigma")
            fFitF.SetParameter(0,preFitF.GetParameter(0))
            fFitF.SetParameter(1,preFitF.GetParameter(1))
            fFitF.SetParameter(2,preFitF.GetParameter(2))
            fFitF.SetLineColor(fullFitColor)

            # Do the second prefit
            hist.Fit(fFitF,"QRS0")
            
            # Final Gaus + pol2
            bkgFuncType = "pol2"
            fullFuncType = "gaus(0)+"+bkgFuncType+"(3)"
            fullFitF = TF1(rad+"FullFitF",fullFuncType,fitMin,fitMax)
            fullFitF.SetParName  (0,"Gaus Constant")
            fullFitF.SetParName  (1,"Gaus Mean")
            fullFitF.SetParName  (2,"Gaus Sigma")
            fullFitF.SetParameter(0,fFitF.GetParameter(0))
            fullFitF.SetParameter(1,fFitF.GetParameter(1))
            fullFitF.SetParameter(2,fFitF.GetParameter(2))
            fullFitF.SetParameter(3,fFitF.GetParameter(3))
            fullFitF.SetParameter(4,fFitF.GetParameter(4))
            fullFitF.SetLineColor(fullFitColor)
        
            # Do the final fit
            hist.Fit(fullFitF,"QRSE0")

            # Fit OK ?
            maxErrorForOK = 1e-4
            fitOK = fullFitF.GetParError(1) < maxErrorForOK
                  
            # Draw the histogram
            hist.Draw()
                            
            # Draw the pre fit
            if not fitOK :
                fFitF.Draw('SAME')
                # Background function
                bkgFunc = TF1( rad+"BkgF", fbkgFuncType, fitMin, fitMax )
                bkgFunc.SetLineColor(bkgColor)
                bkgFunc.SetParameter(0,fFitF.GetParameter(3))
                bkgFunc.SetParameter(1,fFitF.GetParameter(4))
            else:
                fullFitF.Draw('SAME')
                # Background function
                bkgFunc = TF1( rad+"BkgF", bkgFuncType, fitMin, fitMax )
                bkgFunc.SetLineColor(bkgColor)
                bkgFunc.SetParameter(0,fullFitF.GetParameter(3))
                bkgFunc.SetParameter(1,fullFitF.GetParameter(4))
                bkgFunc.SetParameter(2,fullFitF.GetParameter(5))

            # Draw the background shape
            bkgFunc.Draw('SAME')
                       
            # Add Run number to plot
            addRunToPlot(run,[ ("Full Fit",fullFitColor),("Bkg",bkgColor) ] )
            
            # Print to file
            printCanvas()

            # For debugging. Pause here
            if not fitOK :
                raw_input("Press any key to continue ...")
    
            # Results of the fit
            if fitOK :
                result = { 'OK'    : True,
                           'Mean'  : [fullFitF.GetParameter(1),fullFitF.GetParError(1)],
                           'Sigma' : [fullFitF.GetParameter(2),fullFitF.GetParError(2)]
                           }
            else:
                if fFitF.GetParError(1) < maxErrorForOK :
                    result = { 'OK'    : True,
                               'Mean'  : [fFitF.GetParameter(1),fFitF.GetParError(1)],
                               'Sigma' : [fFitF.GetParameter(2),fFitF.GetParError(2)]
                               }
                else:
                    result['Message'] = "Histogram Fit Failed"

            #if result['OK'] :
            #    if ( (rad == 'Rich1Gas' and result['Sigma'][0] > 0.003) or
            #         (rad == 'Rich2Gas' and result['Sigma'][0] > 0.001) ) :
            #        print "Large fitted sigma, please check ? sigma =", result['Sigma'][0]
            #        raw_input("Press any key to continue ...")

        # Close the rootfile
        rootfile.Close()

    # Return the fit result
    return result

def averageCPUTimePerEvent(jobs):

    from ROOT import TH1F

    globals()["imageFileName"] = "CPUTime.png"

    # Root canvas
    rootCanvas()

    # Book a histogram
    minTime = 0
    maxTime = 100000
    cpuHist = TH1F( "NormCPUTime", "Normalised Total CPU Time",
                    100, minTime, maxTime )

    for job in jobs:
        for sjob in job.subjobs:

            # Get the CPU time of this job
            cpuTime = sjob.backend.normCPUTime

            # Fill the plots
            if cpuTime != None :
                fcpuTime = float(cpuTime)
                cpuHist.Fill(fcpuTime)
                if fcpuTime > maxTime :
                    print "Job %s.%s CPU time %s > HistMax %s" % (job.id,sjob.id,fcpuTime,maxTime)
            
    # Print plot to PNG
    cpuHist.Draw('E')
    printCanvas()
    
def addRunToPlot(run,tags=[]):
    from ROOT import TText
    text = TText()
    text.SetNDC()
    text.SetTextSize(0.03)
    x = 0.12
    y = 0.85
    text.DrawText( x, y, "Run "+str(run) )
    for tag in tags:
        if tag[0] != "":
            y -= 0.05
            text.SetTextColor(tag[1])
            text.DrawText( x, y, tag[0] )
    
def getRunLFNData(pickedRuns):
    import pickle
    file = open(pickedRuns,"r")
    RunLFNs = pickle.load(file)
    file.close()
    return RunLFNs

def drange(start,stop,step):
    r = start
    while r < stop:
        yield r
        r += step

def dateTimeString():
    import datetime
    now = datetime.datetime.now()
    return str(now.year)+'-'+str(now.month)+'-'+str(now.day)+'_'+str(now.hour)+'-'+str(now.minute)+'-'+str(now.second)

def createTempOptsFile(name):
    import os
    tempFullPath = os.getcwd()+"/tempOpts"
    if not os.path.exists(tempFullPath) : os.makedirs(tempFullPath)
    return tempFullPath+"/"+name+"_"+dateTimeString()+".py"

#=============================================================================================
