
# Globals
imageFileName = ''
canvas        = None
runInfoCacheName = "RunInfoCache.pck.bz2"
runInfoCacheLoaded = False
runInfoCache  = { }
#histoBase = 'RICH/RiCKResLong/'
histoBase = 'RICH/RiCKResLongTight/'

# ====================================================================================
# Main Methods
# ====================================================================================

## Submits Control Jobs
def submitControlJobs(name="",pickedRuns="Run71813-LFNs.pck.bz2"):
    
    import os
    from Ganga.GPI import ( Job, LHCbDataset, Brunel, File, SplitByFiles, 
                            RootMerger, Dirac )
    
    # Number of target events to process
    nEventsTotal    = 250000
    nFilesMax       = 10000

    RunLFNs = getRunLFNData(pickedRuns)

    # Loop over runs
    for run in sorted(RunLFNs.keys()):

        # LFNs for this run
        lfns = RunLFNs[run]
        if len(lfns)>0 :

            nFiles = len(lfns)
            if nFiles > nFilesMax : nFiles = nFilesMax
            nFilesPerJob = filesPerJob(nFiles)
            nEventsPerJob = nFilesPerJob * nEventsTotal / nFiles
            print "Using", nFiles, "data file(s),", nEventsPerJob, "events per job"

            r = drange( 0.98000, 1.010, 0.001 )
            R1N = [ '%g' % x for x in r ]
    
            r = drange( 1.055, 1.085, 0.001 )
            R2N = [ '%g' % x for x in r ]
        
            for i in range(0,len(R1N)):

                r1 = R1N[i]
                r2 = R2N[i]

                print "(n-1) Scale Rich1 =",r1,"Rich2",r2
            
                # Make a job object
                j = Job( application = Brunel( version = 'v47r6p1' ) )

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
                extraopts.write("Brunel().EvtMax = "+str(nEventsPerJob)+"\n")
                extraopts.close()

                # Job options
                j.application.optsfile = [ File('CKRefractCalib-FullBrunelReco.py'),
                                           File(tmpOptsFile) ]

                # Set the LFNs to run over
                j.inputdata = LHCbDataset(lfns)

                # Split job into 1 file per subjob
                j.splitter = SplitByFiles ( filesPerJob = nFilesPerJob, maxFiles = nFiles )

                # ROOT file output
                rootfiles = [j.name+".root"]

                # Output files
                j.outputfiles = rootfiles

                # Merge the output
                merger = RootMerger( files        = rootfiles,
                                     ignorefailed = True,
                                     overwrite    = True )
                j.postprocessors = [ merger ]

                # Optional input files
                j.inputsandbox = []

                # Dirac backend
                j.backend = Dirac()

                # Enable automatic job re-submission
                j.do_auto_resubmit = True

                # Turn off bulk submission
                j.splitter.bulksubmit = False

                # Submit !!
                j.submit()

## Submits DB calibration jobs
def submitCalibrationJobs(name="",BrunelVer="v47r6p1",pickledRunsList=[]):
    submitRecoJobs(name,BrunelVer,pickledRunsList,"RefInCalib")

## Submit DB Verification Jobs
def submitVerificationJobs(name="",BrunelVer="v47r6p1",pickledRunsList=[]):
    submitRecoJobs(name,BrunelVer,pickledRunsList,"RefInVerify")

## Real underlying method
def submitRecoJobs(name,BrunelVer,pickledRunsList,jobType):

    from Ganga.GPI import ( Job, LHCbDataset, Brunel, File, queues,
                            SplitByFiles, RootMerger, Dirac )
    import time

    # If pickled run data list is empty, create full list
    if len(pickledRunsList) == 0 : pickledRunsList = getPickledRunList()

    # Number of target events to process
    if jobType == "RefInCalib" :
        nEventsTotal  = 200000
        nFilesMax     = 100
    else:
        nEventsTotal  = 200000
        nFilesMax     = 100

    # Base Job Name
    basejobname = jobType
    if name != "" : basejobname += "-"+name
    basejobname += "_BR-"+BrunelVer

    # My input sandboxs
    mySandBox     = [ ]
    mySandBoxLFNs = [ ]

    # List of DB related options to add to extraopts
    dbopts = [ ]

    # Main LHCbCond
    #Cond = "LHCBCOND-2001RePro.db"
    #dbopts += ["CondDB().PartitionConnectionString[\"LHCBCOND\"] = \"sqlite_file:"+Cond+"/LHCBCOND\"\n"]
    ##dbopts += ["LHCbApp().CondDBtag = \"HEAD\"\n"]
    #lfnname = "LFN:/lhcb/user/j/jonrob/DBs/"+Cond
    #uploadFile("databases/"+Cond,lfnname)
    #mySandBoxLFNs += [lfnname]
    
    # Main DDDB
    #Cond = "DDDB-2001RePro.db"
    #dbopts += ["CondDB().PartitionConnectionString[\"DDDB\"] = \"sqlite_file:"+Cond+"/DDDB\"\n"]
    ##dbopts += ["LHCbApp().DDDBtag = \"HEAD\"\n"]
    #lfnname = "LFN:/lhcb/user/j/jonrob/DBs/"+Cond
    #uploadFile("databases/"+Cond,lfnname)
    #mySandBoxLFNs += [lfnname]

    # DDDB Slice
    #Cond = "JulyAugustOnly-OT-DDDB.db"
    #dbopts += ["CondDB().addLayer(dbFile = \""+Cond+"\", dbName = \"DDDB\")\n"]
    ###dbopts += ["LHCbApp().DDDBtag = \"HEAD\"\n"]
    #lfnname = "LFN:/lhcb/user/j/jonrob/DBs/"+Cond
    #uploadFile("databases/"+Cond,lfnname)
    #mySandBoxLFNs += [lfnname]

    # Custom DB slices for both job types (calibration and verification)
    dbFiles = [ ]

    # New 2012 MDMS with per HPD corrections (not used)
    #dbFiles += [ "MDCS-RICH1-PerHPD-14082014" ]

    # New 2011 MDMS with global shift - V1
    #dbFiles += [ "MDCS-RICH1-GLOBAL_SHIFT-FOR2011-11102014" ]
    
    # New 2012 MDMS with global shift - V1
    #dbFiles += [ "MDCS-RICH1-GLOBAL_SHIFT-S21-08082014" ]
    # New 2012 MDMS with global shift - V2
    #dbFiles += [ "MDCS-Global-Shift-08092014" ]
    # New 2012 MDMS with global shift - V3
    #dbFiles += [ "MDCS-RICH1-GLOBAL_SHIFT-25092014" ]
    # New 2012 MDMS with global shift - V4
    #dbFiles += [ "MDCS-RICH1-GLOBAL_SHIFT-FOR2012-11102014" ]

    # 2012 HPD image calibrations with radius values
    #dbFiles += ["2012-NewMDMSCalib-RunAligned-Sobel-Smoothed1.0hours-HPDAlign-17062014"]

    # Antonis's test 2012 DB with fixed Magnification
    #dbFiles += ["MDCS-RICH1_FixedMag_210515"]
    # No MDCS (Zero field)
    #dbFiles += ["MDCS-RICH1_ZeroField_240515"]
    # Run II test - First Scan
    #dbFiles += ["MDCS-RICH1-FirstScans-RunI-IOV-26052015"]
    # Run II test - Second Scan
    #dbFiles += ["MDCS-RICH1-SecondScans-RunI-IOV-26052015"]

    # 2012 test with one alignment for up and down
    #dbFiles += ["2012-MagTest-10062015"]

    # 2015 DBs
    dbFiles += ["Alignment2015"] # Tracking
    dbFiles += ["MDCS-RICH1-SecondScans-26052015"] # MDCS
    dbFiles += ["ResetHPDAlign-13062015"]
    #dbFiles += ["2015-MirrorAlign-V1-It2-15062015"]
    dbFiles += ["2015-MirrorAlign-V2-It3-15062015"]

    # Only for Calibration jobs only
    if jobType == "RefInCalib" :
        dbopts += ["UpdateManagerSvc().ConditionsOverride += [\"Conditions/Environment/Rich1/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n"]
        dbopts += ["UpdateManagerSvc().ConditionsOverride += [\"Conditions/Environment/Rich2/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n"]

    # For verification jobs only, use custom DB Slice for n-1 corrections
    if jobType == "RefInVerify" :
        pass
        #dbFiles += ["RefInCalib-2010RePro-V1_BR-v45r1-15052014"]
        #dbFiles += ["2010RePro-RootFiles-V1-RunAligned-Sobel-Smoothed1.0hours-HPDAlign-15052014"]
        #dbFiles += ["2010RePro-RootFiles-V1-RunAligned-Sobel-Smoothed0.5hours-HPDOcc-16052014"]
        #dbFiles += ["2010MirrorAlign-28052014"]

    # Configure additional DBs
    for dbFile in dbFiles :
        dbopts += ["CondDB().addLayer(CondDBAccessSvc(\""+dbFile+"\",ConnectionString=\"sqlite_file:"+dbFile+".db/LHCBCOND\",DefaultTAG=\"HEAD\"))\n"]
        # Upload to LFNs
        lfnname = "LFN:/lhcb/user/j/jonrob/DBs/"+dbFile+".db"
        if not uploadFile("databases/"+dbFile+".db",lfnname) : return False
        # Add to LFNs sandbox
        mySandBoxLFNs += [lfnname]

    # minimum run number
    minRun = 0

    # Loop over the list of pickled run data files
    print "Submitting jobs for RunData", pickledRunsList
    for pickedRuns in pickledRunsList :

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
            lfns = sorted(RunLFNs[run])
            if len(lfns)>0 :

                # Construct the job name
                jobname = basejobname + "_Run-"+str(run)

                # is this job already submitted ?
                if run < minRun or jobExists(jobname):

                    print "Job", jobname, "already exists ( #", nJob, "of", len(sortedRuns), ")"

                else:

                    # Configure number of files and events per file
                    nFiles = len(lfns)
                    if nFiles > nFilesMax : nFiles = nFilesMax
                    nFilesPerJob = filesPerJob(nFiles)
                    nEventsPerJob = nFilesPerJob * nEventsTotal / nFiles
          
                    # Make a job object
                    j = Job( application = Brunel( version = BrunelVer ),
                             name        = jobname )
                    #j.application.args += ['-T'] # Enable tcmalloc (now done by default)

                    # Custom options for this job
                    tmpOptsFile = createTempOptsFile(j.name)
                    extraopts = open(tmpOptsFile,"w")

                    # Basic additions
                    extraopts.write("from Brunel.Configuration import *\n")
                    extraopts.write("from Gaudi.Configuration import *\n")
                    extraopts.write("from Configurables import Brunel, UpdateManagerSvc, CondDB, CondDBAccessSvc, LHCbApp\n")
                    extraopts.write("HistogramPersistencySvc().OutputFile = \""+j.name+".root\"\n")
                    extraopts.write("Brunel().EvtMax = "+str(nEventsPerJob)+"\n")

                    # Add DB options
                    for dbopt in dbopts : extraopts.write(dbopt)

                    # Close file
                    extraopts.close()

                    # Job options
                    j.application.optsfile = [ File('CKRefractCalib-FullBrunelReco.py'),
                                               File(extraopts.name) ]

                    # Set the LFNs to run over
                    if nFiles == len(lfns):
                        j.inputdata = LHCbDataset(sorted(lfns))
                    else:
                        import random
                        j.inputdata = LHCbDataset(sorted(random.sample(lfns,nFiles)))

                    # Split job into 1 file per subjob
                    j.splitter = SplitByFiles ( filesPerJob = nFilesPerJob, maxFiles = nFiles )

                    # ROOT file output
                    rootfiles = [j.name+".root"]
                    
                    # Output files
                    j.outputfiles = rootfiles

                    # Merge the output
                    merger = RootMerger( files        = rootfiles,
                                         ignorefailed = True,
                                         overwrite    = True )
                    j.postprocessors = [ merger ]

                    # Dirac backend
                    j.backend = Dirac()

                    # Max CPU time
                    #j.backend.settings['CPUTime'] = 50000
                    
                    # Force jobs to go to CERN only
                    #j.backend.settings['Destination'] = 'LCG.CERN.ch'

                    # Optional input files
                    j.inputsandbox             = mySandBox
                    j.backend.inputSandboxLFNs = mySandBoxLFNs

                    # Enable automatic job re-submission
                    j.do_auto_resubmit = True

                    # Turn off bulk submission
                    #j.splitter.bulksubmit = False

                    # Add to jobtree
                    addToJobTree(j,basejobname)

                    # Submit !!
                    print "Submitting Job", j.name, "( #", nJob, "of", len(sortedRuns), ")", time.strftime("%c")
                    print " -> Using", nFiles, "data file(s), max", nFilesPerJob, \
                          "file(s) per subjob,", nEventsPerJob, "events per job"
                    for f in j.inputdata.files : print "  ->", f.name

                    # Submitt now
                    #submitJob(j)
                    # queue the submission
                    queues.add( submitJob, args=(j,) )

def submitJob(j):

    import time
    
    submitOK = False
    submitCount = 0
    while ( not submitOK and submitCount <= 100 ) :
        submitCount = submitCount + 1
        try:
            j.submit()
            # See if any of the sub-jobs are still 'new'
            sj = j.subjobs.select(status='new')
            if len(sj) == 0 : submitOK = True
        except Exception,e:
            submitOK = False
        if not submitOK :
            nSleep = 120
            print "WARNING : Job ", j.name, " not submitted -> Delete and try again after", nSleep, "secs..."
            new_j = j.copy()
            j.remove()
            j = new_j
            time.sleep(nSleep)
            
    if not submitOK :
        print "WARNING : Job ", j.name, " failed to be submitted after", submitCount, "tries."
        j.remove()


def makeAllColumnFits(jobs,rad='Rich1Gas',polarity=''):
    
    if rad == 'Rich1Gas':
        for panel in ['top','bottom']:
            for n in [0,1,2,3,4,5,6]:
                refractiveIndexCalib(jobs,rad,polarity,'Rich1-'+panel+'-Col'+str(n))

    if rad == 'Rich2Gas':
        for panel in ['ASide-left','CSide-right']:
            for n in [0,1,2,3,4,5,6,7,8]:
                refractiveIndexCalib(jobs,rad,polarity,'Rich2-'+panel+'-Col'+str(n))

def refractiveIndexCalib(jobs,rads=['Rich1Gas','Rich2Gas'],polarity='',pdCol='') :

    from ROOT import TFile, TGraphErrors, TF1, TH1, TH1F, gROOT, gStyle, TText, TLatex
    from Ganga.GPI import Job
    from array import array
    import pickle, bz2

    if len(jobs) == 0 : return

    # loop over radiators
    for rad in rads :

        # File name root
        fileNameRoot = rad+"_"+getJobCaliName(jobs[0])

        # Start a PDF file
        fname = "results/"+fileNameRoot
        if polarity != '' : fname += "-"+polarity
        if pdCol    != '' : fname += "-"+pdCol
        globals()["imageFileName"] = fname+".pdf"
        printCanvas('[')

        # Dictionary to store the calibration data
        calibrations = { }

        # Keep tabs on min and max values (for plots)
        minMaxScale = [999.0,-999.0]
        if 'Rich1Gas' == rad :
            #minMaxCKRes = (1.4,1.8)
            minMaxCKRes = (1.4,2.2)
            maxDeltaTheta = 0.03
        else:
            #minMaxCKRes = (0.62,0.72)
            minMaxCKRes = (0.62,0.8)
            maxDeltaTheta = 0.01

        # Raw mean and sigma
        ckmeans  = { }
        cksigmas = { }
        ckraws   = { }
        ckexpect = { }

        if pdCol == '' :
            resPlot = 'ckResAll'
        else:
            resPlot = 'PDCols/'+pdCol

        # Max/min run range
        minMaxRun = [ 0, 99999999 ]
        #minMaxRun = [ 87657, 99999999 ] # Skip first runs of 2011 with bad gas mixtures
        #minMaxRun = [ 101372, 99999999 ] # Second phase of 2011 RePro
        #minMaxRun = [ 103936, 99999999 ] # Third phase of 2011 RePro

        # Bad runs to always skip
        badRuns = [ ]
        badRuns += [ 89537 ]  # Not sure why
        badRuns += [ 111730 ] # RICH signals missing for unknown reason

        # Loop over jobs
        FailedFits = [ ]
        print "Looping over the runs ..."
        for j in jobs :

            # Run Number
            run = int(getInfoFromJob(j,'Run'))
            if ( run >= minMaxRun[0] and run <= minMaxRun[1] and
                 run not in badRuns and
                 ( polarity == '' or polarity == getFieldPolarity(run) ) ) :

                # Root file
                rootfile = getRootFile(j)

                if rootfile :

                    # Turn on fit stats box
                    gStyle.SetOptFit(1011)

                    # Fits
                    fitResultRes = fitCKThetaHistogram(rootfile,run,rad,resPlot)
                    fitResultRaw = fitCKThetaHistogram(rootfile,run,rad,'thetaRec',-1)
                    fitResultExp = fitCKExpectedHistogram(rootfile,run,rad)

                    if fitResultRes['OK'] and fitResultRaw['OK'] and fitResultExp['OK'] :
                        scale = nScaleFromShift(fitResultRes,rad)
                        if scale[0] < minMaxScale[0] : minMaxScale[0] = scale[0]
                        if scale[0] > minMaxScale[1] : minMaxScale[1] = scale[0]
                        calibrations[run] = { "ScaleFactor" : scale, "ThetaShift" : fitResultRes['Mean'] }
                        ckmeans[run]  = fitResultRes['Mean']
                        cksigmas[run] = fitResultRes['Sigma']
                        ckraws[run]   = fitResultRaw['Mean']
                        ckexpect[run] = fitResultExp['Mean']
                    else:
                        FailedFits += [j.id]
                        print "WARNING : fits failed for run", run
                        print "        : CK resolution :", fitResultRes['Message']
                        print "        : CK theta      :", fitResultRaw['Message']
                        print "        : CK expected   :", fitResultExp['Message']

                    # Close the root file
                    rootfile.Close()

            else:
                print " -> Skipping run", run

        # Write out calibrations to a pickled python file
        calibfilename = fname+".pck.bz2"
        print "Writting calibrations to", calibfilename
        file = bz2.BZ2File(calibfilename,"w")
        pickle.dump(calibrations,file)
        file.close()

        nBins = 100

        # 1D Plot of scale factors
        scaleHist = TH1F( "scaleFactors", rad+" (n-1) Scale Factors",
                          nBins, 0.999*minMaxScale[0], 1.0001*minMaxScale[1] )
        scaleHist.GetXaxis().SetTitle("(n-1) corrections")
        scaleHist.GetXaxis().SetTitleOffset(1.5)
        scaleHist.GetYaxis().SetTitle("Entries")

        # 1D Plot of theta shifts
        dThetaHist = TH1F( "deltaTheta", rad+" <Delta CK Theta>",
                           nBins, -1.001*maxDeltaTheta, 1.001*maxDeltaTheta )
        dThetaHist.GetXaxis().SetTitle("<Delta CK Theta> / mrad")
        dThetaHist.GetXaxis().SetTitleOffset(1.5)
        binSize = (2*maxDeltaTheta)/nBins
        dThetaHist.GetYaxis().SetTitle("Entries / ( "+str(binSize) + " mrad )")

        # 1D Plot of Fitted CK resolutions
        ckResHist = TH1F( "ckRes", rad+" Delta CK Theta Resolution",
                          nBins, minMaxCKRes[0], minMaxCKRes[1] )
        ckResHist.SetTitle( " " )
        ckResHist.GetXaxis().SetTitle(rad+" Cherenkov Theta Resolution / mrad")
        ckResHist.GetXaxis().SetTitleOffset(1.5)
        binSize = (minMaxCKRes[1]-minMaxCKRes[0])/nBins
        ckResHist.GetYaxis().SetTitle("Entries / ( "+str(binSize) + " mrad )")
        ckResHist.SetStats(0)

        # Open text file for shifts
        textFileName = fname+".txt"
        print "Opening text file", textFileName
        textShifts = open(textFileName,'w')
        writeLegionsToTextFile(textShifts)

        # TText object
        text = TLatex()
        text.SetNDC()
        text.SetTextSize(0.03)

        # For plots (manually make sure sorted by run)
        runs      = array('d')
        runsErr   = array('d')
        scales    = array('d')
        scalesErr = array('d')
        means     = array('d')
        meansErr  = array('d')
        sigmas    = array('d')
        sigmasErr = array('d')
        ckraw     = array('d')
        ckrawErr  = array('d')
        ckexp     = array('d')
        ckexpErr  = array('d')
        for run in sorted(calibrations.keys()):
            scale   = calibrations[run]["ScaleFactor"]
            ckmean  = ckmeans[run]
            cksigma = cksigmas[run]
            raw     = ckraws[run]
            exp     = ckexpect[run]
            # Write to text file
            writeInfoToTextFile(textShifts,run,raw,exp,ckmean,cksigma,scale)
            # plots
            runs.append(float(run))
            runsErr.append(0.0)
            scales.append(scale[0])
            scalesErr.append(scale[1])
            means.append(1000*ckmean[0])
            meansErr.append(1000*ckmean[1])
            sigmas.append(1000*cksigma[0])
            sigmasErr.append(1000*cksigma[1])
            ckraw.append(raw[0])
            ckrawErr.append(raw[1])
            ckexp.append(exp[0])
            ckexpErr.append(exp[1])
            # Fill 1D histo(s)
            scaleHist.Fill(scale[0])
            ckResHist.Fill(1000*cksigma[0])
            dThetaHist.Fill(1000*ckmean[0])

        # Make the plots
        if len(runs) > 0 :

            linearFit = TF1("AverageFit","pol0",runs[0],runs[len(runs)-1])
            linearFit.SetParName(0,"Mean")

            ckrawTrend = TGraphErrors( len(runs),runs,ckraw,runsErr,ckrawErr )
            ckrawTrend.SetTitle( rad+" Peak CK Theta by Run" )
            ckrawTrend.GetXaxis().SetTitle("LHCb Run Number")
            ckrawTrend.GetYaxis().SetTitle("Peak CK Theta / mrad")
            ckrawTrend.Draw("ALP")
            printCanvas()

            ckexpTrend = TGraphErrors( len(runs),runs,ckexp,runsErr,ckexpErr )
            ckexpTrend.SetTitle( rad+" <Expected CK Theta> by Run" )
            ckexpTrend.GetXaxis().SetTitle("LHCb Run Number")
            ckexpTrend.GetYaxis().SetTitle("<Expected CK Theta> / mrad")
            ckexpTrend.Draw("ALP")
            printCanvas()

            meanFitFunc = TF1("CKMean"+rad,"gaus",-maxDeltaTheta,maxDeltaTheta)
            dThetaHist.Fit(meanFitFunc,"QR")
            dThetaHist.Draw('E')
            printCanvas()

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

            # Turn off fit stats box
            gStyle.SetOptFit(0)

            sigmaTrend = TGraphErrors( len(runs),runs,sigmas,runsErr,sigmasErr )
            #sigmaTrend.SetTitle( rad+" Cherenkov Theta Resolution by Run" )
            sigmaTrend.SetTitle( " " )
            sigmaTrend.GetXaxis().SetTitle("LHCb Run Number")
            sigmaTrend.GetXaxis().SetTitleOffset(1.5)
            sigmaTrend.GetYaxis().SetTitle(rad+" Cherenkov Theta Resolution / mrad")
            sigmaTrend.GetYaxis().SetTitleOffset(1.5)
            fitRes = sigmaTrend.Fit(linearFit,"MQRS")
            sigmaTrend.Draw("AP")
            text.DrawLatex( 0.44, 0.82, "#LT#sigma_{#Delta#theta}#GT = " +
                            str("%0.3f"%fitRes.Value(0)) +
                            #" +- " + str("%0.3f"%fitRes.ParError(0)) +
                            " mrad" )
            if rad == "Rich1Gas" :
                text.DrawText( 0.15, 0.85, "a)" )
            else:
                text.DrawText( 0.15, 0.85, "b)" )
            printCanvas()

            fitFunc = TF1("Scale"+rad,"gaus",minMaxScale[0],minMaxScale[1])
            scaleHist.Fit(fitFunc,"QR")
            scaleHist.Draw('E')
            printCanvas()

            scaleTrend = TGraphErrors( len(runs),runs,scales,runsErr,scalesErr )
            scaleTrend.SetTitle( rad+" (n-1) corrections by Run" )
            scaleTrend.GetXaxis().SetTitle("LHCb Run Number")
            scaleTrend.GetYaxis().SetTitle("(n-1) Scale Factor")
            scaleTrend.Fit(linearFit,"QRS")
            scaleTrend.Draw("ALP")
            printCanvas()

            # Turn on fit stats box
            gStyle.SetOptFit(1011)

        # Close PDF file
        printCanvas(']')

        if len(FailedFits) > 0 :
            print "WARNING :", len(FailedFits), "histogram fits failed. Job IDs = ", FailedFits

def writeLegionsToTextFile(file):
    text = "Run Fill Description StartDate StartTime StopDate StopTime CKRaw CKRawErr CKExp CKExpErr CKMean CKMeanErr CKSigma CKSigmaErr ScaleFactor ScaleFactorErr"
    file.write(text+"\n")

def writeInfoToTextFile(file,run,ckraw,ckexp,ckmean,cksigma,scale):
    runInfo = getRunInformation(run)
    fill = runInfo['FillNumber']
    desc = runInfo['DataTakingDescription']
    text = ( str(run) + " " + str(fill) + " " + desc
             + " " + str(runInfo['RunStart']) + " " + str(runInfo['RunEnd'])
             + " " + str(ckraw[0])   + " " + str(ckraw[1])
             + " " + str(ckexp[0])   + " " + str(ckexp[1])
             + " " + str(ckmean[0])  + " " + str(ckmean[1])
             + " " + str(cksigma[0]) + " " + str(cksigma[1])
             + " " + str(scale[0])   + " " + str(scale[1]) )
    file.write(text+"\n")
    
def refractiveIndexControl(jobs,rad='Rich1Gas'):
    
    from Ganga.GPI import Job
    from ROOT import TGraphErrors
    from array import array

    # Start a PDF file
    globals()["imageFileName"] = "results/"+rad+"-RefIndexControl.pdf"
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
        rootfile = getRootFile(j)
  
        print "Fitting job", rad, "Run =", run, "index =", refIndex
        fitResult = fitCKThetaHistogram(rootfile,run,rad)

        if fitResult['OK'] :
            x.append(float(refIndex))
            xe.append(0.0)
            y.append(float(fitResult['Mean'][0]))
            ye.append(float(fitResult['Mean'][1]))
        else:
            print " -> Fit failed -", fitResult['Message']

        rootfile.Close()

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

    globals()["imageFileName"] = "results/"+rad+"-ExpectCKtheta.pdf"
    printCanvas('[')

    runs      = array('d')
    runsErr   = array('d')
    exp       = array('d')
    expErr    = array('d')

    for j in jobs :

        # Run number
        run = int(getInfoFromJob(j,'Run'))

        # Root file
        rootfile = getRootFile(j)

        # get info from plot
        result = fitCKExpectedHistogram(rootfile,run,rad)
        if result['OK'] :

            # Mean of the histo
            mean = result['Mean'][0]
            err  = result['Mean'][1]
            
            runs.append(float(run))
            runsErr.append(0.0)
            exp.append(mean)
            expErr.append(err)
    
        rootfile.Close()
                
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
    globals()["imageFileName"] = "results/"+rad+"-RecoCKtheta.pdf"
    printCanvas('[')

    runs      = array('d')
    runsErr   = array('d')
    reco      = array('d')
    recoErr   = array('d')

    for j in jobs :

        run      = int(getInfoFromJob(j,'Run'))
        rootfile = getRootFile(j)

        fitResult = fitCKThetaHistogram(rootfile,run,rad,'thetaRec')

        if fitResult['OK'] :
            runs.append(float(run))
            runsErr.append(0.0)
            reco.append(float(fitResult['Mean'][0]))
            recoErr.append(float(fitResult['Mean'][1]))
        else:
            print " -> Fit failed -", fitResult['Message']

        rootfile.Close()
            
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

def uploadFile(pfn,lfn,sites=['CERN-USER','RAL-USER','IN2P3-USER',
                              'GRIDKA-USER','PIC-USER','CNAF-USER']):
    #   ,'NIKHEF-USER'
    from Ganga.GPI import PhysicalFile, LogicalFile
    import time

    if len(sites) == 0 :
        print 'ERROR : No sites requested'
        return False

    # Check if file has any replicas to start with
    res = LogicalFile(lfn).getReplicas()
    OK = True
    if len(res) < len(sites) :

        print "Uploading", pfn, "to", sites[0], lfn

        # First upload one copy
        if len(res) == 0 :
            newlfn = PhysicalFile(pfn).upload(lfn,sites[0])
            time.sleep(30)
            if len(newlfn.getReplicas()) == 0:
                print "Problem uploading ..."
                OK = False

        if OK :
            print " -> Upload SUCCESSFUL"
            # Replicate to requested sites
            for site in sites :
                try:
                    print '  -> Replicating to', site
                    LogicalFile(lfn).replicate(site)
                except Exception,e:
                    print '   -> ERROR Replication FAILED'
                    
    else:
        print lfn, "already exists as", res

    return OK

def uploadDBs(dbFiles=[]):
    for dbFile in dbFiles :
        lfnname = "LFN:/lhcb/user/j/jonrob/DBs/"+dbFile
        if not uploadFile("databases/"+dbFile,lfnname) : return False
    return True

def addToJobTree(j,dir):
    from Ganga.GPI import jobtree
    fulldir = "/RichCalibration/"+dir
    if not jobtree.exists(fulldir) : jobtree.mkdir(fulldir)
    jobtree.cd(fulldir)
    jobtree.add(j,fulldir)
    
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
    splits = j.name.split('_')
    cName = splits[0]
    if len(splits) == 3 : cName += "_"+splits[1]
    return cName

def loadDict(filename):
    import pickle, os, bz2
    data = { }
    if os.path.exists(filename) :
        file = bz2.BZ2File(filename,"r")
        data = pickle.load(file)
        file.close()
    return data

def pickleDict(filename,data):
    import pickle, os, bz2
    file = bz2.BZ2File(filename,"w")
    pickle.dump(data,file)
    file.close()

def loadRunInfoCache():
    if not globals()["runInfoCacheLoaded"]:
        cachename = globals()["runInfoCacheName"]
        #print "Loading Run Info cache -", cachename
        globals()["runInfoCache"] = loadDict(cachename)
        globals()["runInfoCacheLoaded"] = True
    return globals()["runInfoCache"]

def saveRunInfoCache():
    cachename = globals()["runInfoCacheName"]
    #print "Saving Run Info cache  -", cachename
    pickleDict(cachename,globals()["runInfoCache"])

def queryBKDB(run):
    
    print "Getting information for run", run, "from BK API... Be patient..."
    
    from Ganga.GPI import diracAPI
    import time

    # "from DIRAC.Core.Base import Script; Script.parseCommandLine();" +
    cmd = ( "from LHCbDIRAC.BookkeepingSystem.Client.BookkeepingClient import BookkeepingClient;" +
            "output( BookkeepingClient().getRunInformations("+str(run)+") )" )

    res = { 'OK' : False }
    nTries = 0
    while ( not res.has_key('OK') or not res['OK'] ) and nTries < 10:
        nTries = nTries + 1
        
        # Get information from Dirac
        res = diracAPI(cmd)
        print res

        if not res.has_key('OK') or not res['OK'] :
            print " -> Problem querying DB - Will try again after 5 secs ..."
            time.sleep(5)

    if res['OK'] :
        
        # cached info
        runInfoCache = loadRunInfoCache()
        # Save
        saveBKQuery(run,res,runInfoCache)
        
    # return result
    return res

def saveBKQuery(run,res,runInfoCache):
    if len(res.keys()) > 0 and res.has_key('OK'):
        if res['OK']:
            runInfoCache[run] = res
            # Save the cache file
            saveRunInfoCache()

def getRunInformation(run):
    
    # cached info
    runInfoCache = loadRunInfoCache()

    # If cache has entry for this run use that. Otherwise fill.
    if run in runInfoCache.keys():
        res = runInfoCache[run]
    else:
        res = queryBKDB(run)
 
    # Checks
    if not isinstance(res,dict):
        print "ERROR : DB query result is not a dictionary... Retry..."
        res = queryBKDB(run)
    if not res.has_key('Value'):
        print "ERROR : DB query result has no key 'Value'... Retry..."
        res = queryBKDB(run)
    if not res.has_key('OK') or not res['OK']:
        print "ERROR : DB query result is not OK... Retry..."
        res = queryBKDB(run)
    if not isinstance(res['Value'],dict):
        print "ERROR : DB query result['Value'] is not a dictionary... Retry..."
        res = queryBKDB(run)

    # return the result
    info = { }
    if res['OK'] : info = res['Value']
    return info
                        
def getListOfJobs(tag,name,BrunelVer,statuscodes,MinRun=0,MaxRun=99999999,desc=""):
    from Ganga.GPI import Job, jobs
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
                            runInfo = getRunInformation(run)
                            if runInfo['DataTakingDescription'] == desc:
                                dict[run] = j
    for d in sorted(dict.keys()) : cJobs += [dict[d]]
    return cJobs

def getCalibrationJobList(name="",BrunelVer="v47r6p1",statuscodes=['completed'],
                          MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInCalib',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def getVerificationJobList(name="",BrunelVer="v47r6p1",statuscodes=['completed'],
                           MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInVerify',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def getControlJobList(name="",BrunelVer="v47r6p1",statuscodes=['completed'],
                      MinRun=0,MaxRun=99999999,desc=""):
    return getListOfJobs('RefInControl',name,BrunelVer,statuscodes,MinRun,MaxRun,desc)

def nScaleFromShift(shift,rad='Rich1Gas'):

    # As of RICH S/W meeting 3/9/2010
    #slope = 38.2388535346
    #if rad == 'Rich2Gas': slope = 68.2

    # 2011-NewTkRichAlign-V1
    #slope = 38.0
    #if rad == 'Rich2Gas': slope = 68.5

    # Final tuning for 2011 repro
    slope = 38.1
    if rad == 'Rich2Gas': slope = 65.25
    
    # Compute the scale factor and its error
    result = 1.0 + (shift['Mean'][0]*slope)
    error  = shift['Mean'][1]*slope
    
    # Return the values
    return [result,error]

def getRootFilePath(j):
    outfiles = [ j.outputdir + f.namePattern for f in j.outputfiles ]
    filename = ""
    if len(outfiles) > 0 : filename = outfiles[0]
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
    print "Dumping", len(names), "ROOT file name(s) to", filename
    for name in names : file.write(name+"\n")
    file.close()
    
def getRootFile(j):
    import os
    from ROOT import TFile
    file = None
    filename = getRootFilePath(j)
    if filename != "" :
        if os.path.exists(filename):
            #print "Opening file", filename
            file = TFile( filename )
        else:
            print "ERROR :", filename, "does not exist"
    else:
        print "ERROR Accessing ROOT file for job", j.id
    return file

def removeSubJobOutputData(js):
    import os, shutil
    for j in js:
        if j.status == "completed":
            rootFile = getRootFile(j)
            if rootFile != None:
                rootFile.Close()
                print "Removing subjob output data for job", j.id, j.name
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
    import os
    canvas = rootCanvas()
    imageType = imageFileName.split(".")[1]
    if tag == "[" :
        if os.path.exists(imageFileName) : os.remove(imageFileName)
        print "Opening file", imageFileName
    if tag != "[" and tag != "]" : canvas.Update()
    canvas.Print(imageFileName+tag,imageType)

def fitCKExpectedHistogram(rootfile,run,rad='Rich1Gas'):

    from ROOT import TF1, TH1, TText, gROOT

    # Default return result
    result = { 'OK' : False, "Message" : "No Message" }

    # Load the root file for this job
    if rootfile == None :

        result['Message'] = "Failed to open ROOT file"

    else:

        # Parameters
        minEntries = 50
        
        # Get the histogram
        histName = globals()["histoBase"]+rad+'/thetaExpect'
        hist = rootfile.Get(histName)
        if not hist:
            
            print "ERROR : Could not access histogram", histName
            
        else:

            # Draw
            hist.Draw()
            # Add Run number to page
            addRunToPlot(run)
            # Print
            #printCanvas()
                
            # Basic check on the histograms before fitting
            entries = hist.GetEntries()
            if entries < minEntries :

                result['Message'] = "Too few histogram entries"

            else:

                # Mean of the histo
                mean    = hist.GetMean()
                meanerr = hist.GetMeanError()
                
                result = { 'Message' : "Fit OK",
                           'OK'      : True,
                           'Mean'    : [mean,meanerr]
                           }

    return result

def fitCKForFile(filename,plot='ckResAll',outfile="CKFit.pdf"):

    from ROOT import TFile, TH1F, TF1, TH1, TText, gROOT

    file = TFile(filename)

    if file == None :

        print "Failed to open ROOT file", filename

    else:

        # Start a PDF file
        globals()["imageFileName"] = "results/"+outfile
        printCanvas('[')

        fitResult1 = fitCKThetaHistogram(file,0,'Rich1Gas',plot)
        print fitResult1
        fitResult2 = fitCKThetaHistogram(file,0,'Rich2Gas',plot)
        print fitResult2

        # Close PDF file
        printCanvas(']')

        file.Close()

def checkCKThetaStats(hist,minEntries=5000):
    return hist.GetEntries() >= minEntries
        
def fitCKThetaHistogram(rootfile,run,rad='Rich1Gas',plot='ckResAll',nPolFull=4):

    from ROOT import TH1F, TF1, TH1, TText, gROOT

    # Default return result
    result = { 'OK' : False, "Message" : "No Message" }

    # Load the root file for this job
    if rootfile == None :

        result['Message'] = "Failed to open ROOT file"

    else:

        # Get the histogram
        histName = globals()["histoBase"]+rad+'/'+plot
        hist = rootfile.Get(histName)
        if not hist :
            
            print "ERROR : Could not access histogram", histName

        else:

            # Basic check on the histograms before fitting
            if not checkCKThetaStats(hist) :

                result['Message'] = "Too few histogram entries (" + str(hist.GetEntries()) + ")"
                
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
                # Starting params
                preFitF.SetParameter(1,0)
                if rad == 'Rich1Gas' :
                    preFitF.SetParameter(2,0.0015)
                else:
                    preFitF.SetParameter(2,0.0007)
 
                # Do the pre fit with just a Gaussian
                hist.Fit(preFitF,"QRS0")

                # Full Fitting range
                if rad == 'Rich1Gas' :
##                     fitMax = xPeak+0.0069
##                     fitMin = xPeak-0.0088
                    fitMax =  0.0075
                    fitMin = -0.0075
                else:
##                     fitMax = xPeak+0.0036
##                     fitMin = xPeak-0.0044
                    fitMax =  0.0035
                    fitMin = -0.0039

                # Loop over pol fits up to the final
                lastFitF = preFitF
                bestFitF = preFitF
                bestNPol = nPolFull
                fitOK    = True
                if nPolFull>0 :
                    for nPol in xrange(1,nPolFull+1):
                        fFuncType = "gaus(0)+pol"+str(nPol)+"(3)"
                        fFitF = TF1(rad+"FitF"+str(nPol),fFuncType,fitMin,fitMax)
                        fFitF.SetLineColor(fullFitColor)
                        fFitF.SetParName(0,"Gaus Constant")
                        fFitF.SetParName(1,"Gaus Mean")
                        fFitF.SetParName(2,"Gaus Sigma")
                        nParamsToSet = 3
                        if nPol > 1 : nParamsToSet = 3+nPol
                        for p in xrange(0,nParamsToSet) :
                            fFitF.SetParameter(p,lastFitF.GetParameter(p))
                        hist.Fit(fFitF,"MQRSE0")
                        lastFitF = fFitF
                        # Fit OK ?
                        maxErrorForOK = 1e-3
                        # gMinuit = gROOT.GetGlobal( "gMinuit", 1 )
                        # print gMinuit.GetStatus()
                        fitOK = fFitF.GetParError(1) < maxErrorForOK
                        if fitOK :
                            bestFitF = fFitF
                            bestNPol = nPol
                        else:
                            if nPol == nPolFull:
                                # Use last good fit
                                print "Pol", nPol, "fit failed...."
                                hist.Fit(fFitF,"RSE0")
                                print " -> Going back to Pol", nPol-1, "fit"
                                fitOK = True
                            if nPol > 1 : break
                  
                # Draw the histogram
                hist.Draw()

                # Draw the full fit
                bestFitF.Draw('SAME')
                
                # Background function
                if bestNPol > 0 :
                    bkgFunc = TF1( rad+"BkgF", "pol"+str(bestNPol), fitMin, fitMax )
                    bkgFunc.SetLineColor(bkgColor)
                    for n in range(0,bestNPol+1):
                        bkgFunc.SetParameter(n,bestFitF.GetParameter(n+3))
                    # Draw the background shape
                    bkgFunc.Draw('SAME')
                       
                # Add Run number to plot
                if bestNPol > 0 :
                    addRunToPlot(run,[ ("Signal+Bkg Fit",fullFitColor),
                                       ("Bkg pol"+str(bestNPol),bkgColor) ] )
                else:
                    addRunToPlot(run)
            
                # Print to file
                if plot != 'thetaRec' : printCanvas()
    
                # Results of the fit
                if fitOK :
                    result = { 'Message' : "Fit OK",
                               'OK'      : True,
                               'Mean'    : [bestFitF.GetParameter(1),bestFitF.GetParError(1)],
                               'Sigma'   : [abs(bestFitF.GetParameter(2)),bestFitF.GetParError(2)]
                               }
                else:
                    result['Message'] = "Histogram Fit Failed"

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

def getFieldPolarity(run):

    runInfo = getRunInformation(run)
    desc = runInfo['DataTakingDescription']
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
    import pickle, bz2
    file = bz2.BZ2File(pickedRuns,"r")
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

def mtime(filename):
    import os
    return os.stat(filename).st_mtime

def getPickledRunList():
    import glob
    return sorted(glob.glob('RunData/*.pck.bz2'),key=mtime)

def resubmitJobsWithBadRootFile(cjobs=[],includeLowStatsCheck=False):

    # Try and help with weird seg. fault ...
    from ROOT import TFile
    
    djobs = [ ]

    if len(cjobs) == 0 :
        from Ganga.GPI import jobs
        cjobs = jobs
    
    for j in cjobs:

        OK = True

        # Root file
        rootfile = getRootFile(j)

        if rootfile :
            
            for rad in ['Rich1Gas','Rich2Gas']:

                if OK :
                    histName = globals()["histoBase"]+rad+'/ckResAll'
                    hist = rootfile.Get(histName)
                    if not hist :
                        print " -> Job", j.id , j.name, "has bad ROOT file."
                        OK = False
                    else:
                        if includeLowStatsCheck :
                            OK = checkCKThetaStats(hist)
                            if not OK : print " -> Job", j.id , "has low stats. ROOT file."

            rootfile.Close()

        else:

            print " -> Job", j.id , "has missing ROOT file."
            OK = False

        if not OK : djobs += [j]

    if len(djobs) == 0 :
        print "No jobs to resubmit"
    else:
        print "Jobs to resubmit :", djobs
        for j in djobs :
            for sj in j.subjobs :
                pass
                sj.force_status('failed')
                sj.resubmit()

def checkInputDataReplicas(lfns):
    OK = True
    for lfn in lfns :
        res = lfn.getReplicas()
        if len(res) == 0 :
            print "ERROR : LFN", lfn.name, "has no replicas"
            OK = False
    return OK

def filesPerJob(nFiles):
    if nFiles == 1  : return 1
    if nFiles == 2  : return 2
    if nFiles == 3  : return 3
    if nFiles == 4  : return 2
    if nFiles == 5  : return 2
    if nFiles == 6  : return 2
    if nFiles == 7  : return 3
    if nFiles == 8  : return 2
    if nFiles == 9  : return 3
    if nFiles == 10 : return 5
    if nFiles == 12 : return 4
    if nFiles == 18 : return 6
    if nFiles < 20  : return 5
    if nFiles < 100 : return 6
    return 10

def removeCalibrationDataSet(name,BrunelVer="v47r6p1"):
    from Ganga.GPI import jobtree, queues
    js = getCalibrationJobList(name,BrunelVer,
                               statuscodes=['completed','running','submitted',
                                            'failed','new','submitting'])
    for j in js : queues.add(j.remove)
    path = '/RichCalibration/RefInCalib-'+name+'_BR-'+BrunelVer
    if jobtree.exists(path) : jobtree.rm(path)
    jobtree.cd('/RichCalibration')

def removeVerificationDataSet(name,BrunelVer="v47r6p1"):
    from Ganga.GPI import jobtree, queues
    js = getVerificationJobList(name,BrunelVer,
                                statuscodes=['completed','running','submitted'
                                             ,'failed','new','submitting'])
    for j in js : queues.add(j.remove)
    path = '/RichCalibration/RefInVerify-'+name+'_BR-'+BrunelVer
    if jobtree.exists(path) : jobtree.rm(path)
    jobtree.cd('/RichCalibration')

#=============================================================================================
