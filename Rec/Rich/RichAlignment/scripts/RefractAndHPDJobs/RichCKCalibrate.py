
# Globals
imageFileName = ''
canvas        = None

# ====================================================================================
# Main Methods
# ====================================================================================

## Submits Control Jobs
def submitControlJobs(pickedRuns="Run71813-LFNs.pck"):
    
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
                j = Job( application = Brunel( version = 'v37r6p1' ) )

                # name
                j.name = "RefractIndexControl_Run-"+str(run)+"_R1-"+r1+"_R2-"+r2
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
def submitCalibrationJobs(pickedRunsList=["RunLFNs.pck"]):
    submitRecoJobs(pickedRunsList,"RefractIndexCalib")

## Submit DB Verification Jobs
def submitVerificationJobs(pickedRunsList=["RunLFNs.pck"]):
    submitRecoJobs(pickedRunsList,"RefractIndexVerify")

## Real underlying method
def submitRecoJobs(pickedRunsList,jobType):

    import os
    from Ganga.GPI import ( Job, LHCbDataset, Brunel, File, DiracSplitter, 
                            SmartMerger, Dirac )

    # Number of target events to process
    if jobType == "RefractIndexCalib" :
        nEventsTotal = 250000
        nFilesMax    = 100
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

                nFiles = len(lfns)
                if nFiles > nFilesMax : nFiles = nFilesMax
                nEventsPerFile = nEventsTotal / nFiles
                print "Using", nFiles, "data files,", nEventsPerFile, "events per file"
          
                # Make a job object
                j = Job( application = Brunel( version = 'v37r6p1' ) )

                # name
                j.name = jobType+"_Run-"+str(run)
                print "Submitting Job", j.name, "( #", nJob, "of", len(sortedRuns), ")"

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
                if jobType == "RefractIndexCalib" :
                    extraopts.write("from Configurables import UpdateManagerSvc\n")
                    extraopts.write("ums = UpdateManagerSvc()\n")
                    extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich1/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n")
                    extraopts.write("ums.ConditionsOverride += [\"Conditions/Environment/Rich2/RefractivityScaleFactor := double CurrentScaleFactor = 1.0;\"]\n")

                # For verification jobs, use custom DB Slice
                if jobType == "RefractIndexVerify" :
                    dbFile = "NewRichCKRefIndexCalib.db"
                    extraopts.write("from Gaudi.Configuration import *\n")
                    extraopts.write("from Configurables import CondDB, CondDBAccessSvc\n")
                    extraopts.write("CondDB().addLayer(CondDBAccessSvc(\"RichCKCond\",ConnectionString=\"sqlite_file:"+dbFile+"/LHCBCOND\",DefaultTAG=\"HEAD\"))\n")
                    mySandBox += [dbFile]

                # Close file
                extraopts.close()

                # Job options
                j.application.optsfile = [ File('CKRefractCalib-FullBrunelReco.py'),
                                           File(extraopts.name) ]

                # Set the LFNs to run over
                j.inputdata = LHCbDataset(lfns)

                # Split job into 1 file per subjob
                j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = nFiles )

                # Merge the output
                j.merger = SmartMerger( files = [j.name+".root"],
                                        ignorefailed = True, overwrite = True )

                # Optional input files
                j.inputsandbox = mySandBox

                # CPU limit for Dirac
                maxCPUlimit = 50000

                # Dirac backend
                j.backend = Dirac()
                #j.backend = Dirac( settings = {'CPUTime':maxCPUlimit} )

                # Submit !!
                j.submit()

def refractiveIndexCalib(jobs,rad='Rich1Gas'):

    from Ganga.GPI import Job
    from ROOT import TGraphErrors, TH1F, TF1
    from array import array
    import pickle

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RefIndexCalib.ps"
    printCanvas('[')

    # Dictionary to store the calibration data
    calibrations = { }

    # Keep tabs on min and max scale factor (for plots)
    minMaxScale = [999.0,-999.0]

    for j in jobs :

        run = int(getInfoFromJob(j,'Run'))

        print "Fitting job", rad, "Run =", run
        fitResult = getPeakPosition(j,rad)

        if fitResult[0] == 'OK':
            scale = nScaleFromShift(fitResult,rad)
            print " -> Fit OK. Scale factor =", scale
            calibrations[run] = scale
            if scale[0] < minMaxScale[0] : minMaxScale[0] = scale[0]
            if scale[0] > minMaxScale[1] : minMaxScale[1] = scale[0]

    # Write out calibrations to a pickled python file
    file = open(rad+"-RefIndexCalib.pck","w")
    pickle.dump(calibrations,file)
    file.close()

    # 1D Plot of scale factors
    scaleHist = TH1F( "scaleFactors", rad+" (n-1) Scale Factors",
                      100, 0.999*minMaxScale[0], 1.0001*minMaxScale[1] )
  
    # For plots (manually make sure sorted by run)
    runs      = array('d')
    runsErr   = array('d')
    scales    = array('d')
    scalesErr = array('d')
    for run in sorted(calibrations.keys()):
        # For scatter plot
        scale = calibrations[run]
        runs.append(float(run))
        runsErr.append(0.0)
        scales.append(scale[0])
        scalesErr.append(scale[1])
        # Fill 1D histo
        scaleHist.Fill(scale[0])

    # Fit and Print the histo
    fitFunc = TF1("Scale"+rad,"gaus",minMaxScale[0],minMaxScale[1])
    scaleHist.Fit(fitFunc,"R")
    scaleHist.Draw('E')
    printCanvas()

    # Make a plot
    graph = TGraphErrors( len(runs),runs,scales,runsErr,scalesErr )
    graph.SetTitle( rad+" (n-1) corrections by Run" )
    graph.GetXaxis().SetTitle("Run Number")
    graph.GetYaxis().SetTitle("(n-1) Scale Factor")
    graph.Draw("AL*")
    printCanvas()
       
    # Close PDF file
    printCanvas(']')
    
def refractiveIndexControl(jobs,rad='Rich1Gas'):
    
    from Ganga.GPI import Job
    from ROOT import TGraphErrors
    from array import array

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RefIndexControl.ps"
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
        fitResult = getPeakPosition(j,rad)
        print fitResult

        if fitResult[0] == 'OK':
            x.append(float(refIndex))
            xe.append(0.0)
            y.append(float(fitResult[1]))
            ye.append(float(fitResult[2]))

    graph = TGraphErrors( len(x),x,y,xe,ye )
    graph.SetTitle( rad+" (n-1) Control plot" )
    graph.GetXaxis().SetTitle("(n-1) Scale factor")
    graph.GetYaxis().SetTitle("CK theta shift")
    graph.Draw("AL*")
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

    globals()["imageFileName"] = rad+"-ExpectCKtheta.ps"
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
    graph.Draw("AL*")
    printCanvas()
       
    # Close output file
    printCanvas(']')

def recoCKTheta(jobs,rad='Rich1Gas'):

    from Ganga.GPI import Job
    from ROOT import TGraphErrors
    from array import array
    import pickle

    # Start a PDF file
    globals()["imageFileName"] = rad+"-RecoCKtheta.ps"
    printCanvas('[')

    runs      = array('d')
    runsErr   = array('d')
    reco      = array('d')
    recoErr   = array('d')

    for j in jobs :

        run = int(getInfoFromJob(j,'Run'))

        print "Fitting job", rad, "Run =", run
        fitResult = getPeakPosition(j,rad,'thetaRec')

        if fitResult[0] == 'OK':
            runs.append(float(run))
            runsErr.append(0.0)
            reco.append(float(fitResult[1]))
            recoErr.append(float(fitResult[2]))
                 
    # Make a plot
    graph = TGraphErrors( len(runs),runs,reco,runsErr,recoErr )
    graph.SetTitle( rad+" Peak Reco CK Theta by Run" )
    graph.GetXaxis().SetTitle("Run Number")
    graph.GetYaxis().SetTitle("CK Theta Peak / mrad")
    graph.Draw("AL*")
    printCanvas()
       
    # Close PDF file
    printCanvas(']')

# ====================================================================================
# Utility Methods
# ==================================================================================== 

def getInfoFromJob(j,info='Run'):
    run = 0
    split = ''
    splits = j.name.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == info : run = s[1]
    return run

def getListOfJobs(tag,statuscodes):
    from Ganga.GPI import Job, jobs
    cJobs = [ ]
    for j in jobs :
        if j.status in statuscodes :
            if j.name.split('_')[0] == tag :
                cJobs += [j]
                #print "Selected job", j.id, j.name
    return cJobs

def getCalibrationJobList(statuscodes=['completed']):
    return getListOfJobs('RefractIndexCalib',statuscodes)

def getVerificationJobList(statuscodes=['completed']):
    return getListOfJobs('RefractIndexVerify',statuscodes)

def getControlJobList(statuscodes=['completed']):
    return getListOfJobs('Control',statuscodes)

def nScaleFromShift(shift,rad='Rich1Gas'):
    slope = 38.2388535346
    #if rad == 'Rich2Gas': slope = 69.6869097141
    #if rad == 'Rich2Gas': slope = 69.4
    if rad == 'Rich2Gas': slope = 68.2
    result = 1.0 + (shift[1]*slope)
    error  = shift[2]*slope
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
    from ROOT import TFile
    file = None
    filename = getRootFilePath(j)
    if filename != "" :
        print "Opening ROOT File", filename
        file = TFile( filename )
    else:
        print "ERROR Accessing ROOT file for job", j.id
    return file

def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        rootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",1000,750)
    return globals()["canvas"]

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
        
def getPeakPosition(j,rad='Rich1Gas',plot='ckResAll'):

    from ROOT import TF1, TH1, TText, gMinuit

    # Parameters
    minEntries = 100000

    # Default return result failed ...
    result = ['Failed',0,0]

    # Run number
    run = getInfoFromJob(j,'Run')

    # Load the root file for this job
    rootfile = getRootFile(j)
    if rootfile != None :

        # Get the histogram
        histName = 'RICH/RiCKResLong/'+rad+'/'+plot
        hist = rootfile.Get(histName)

        # Basic check on the histograms before fitting
        entries = hist.GetEntries()
        if entries > minEntries :

            # Get x value of highest content bin
            # (rough estimate of peak position)
            xPeak = hist.GetBinCenter(hist.GetMaximumBin())

            # Pre Fitting range
            delta = 0.0025
            if rad == 'Rich2Gas' : delta = 0.00105
            fitMin = xPeak - delta
            fitmax = xPeak + delta

            # Gaussian function
            preFitFunc = TF1(rad+"FitF","gaus",fitMin,fitmax)

            # Do the pre fit with just a Gaussian
            hist.Fit(preFitFunc,"R")
            preValue = preFitFunc.GetParameter(1)
            preError = preFitFunc.GetParError(1)

            # Add Run number to page
            addRunToPlot(run,"Pre Fit")
            
            # Print to file
            printCanvas()

            # Full Fitting range
            delta = 0.005
            if rad == 'Rich2Gas' : delta = 0.002
            fitMin = xPeak - delta
            fitmax = xPeak + delta

            # Gaus + pol3
            fullFitFunc = TF1(rad+"FitF","gaus(0)+pol3(3)",fitMin,fitmax)
            fullFitFunc.SetParameter(0,preFitFunc.GetParameter(0))
            fullFitFunc.SetParameter(1,preFitFunc.GetParameter(1))
            fullFitFunc.SetParameter(2,preFitFunc.GetParameter(2))
        
            # Do the final fit
            hist.Fit(fullFitFunc,"R")
            fullValue = fullFitFunc.GetParameter(1)
            fullError = fullFitFunc.GetParError(1)
            hist.Draw()

            # Add Run number to page
            addRunToPlot(run,"Full Fit")

            # Print to file
            printCanvas()
    
            # Results of the fit
            maxErrorForOK = 1e-4
            if fullError < maxErrorForOK :
                result = ['OK',fullValue,fullError]
            else:
                if preError < maxErrorForOK :
                    result = ['OK',preValue,preError]
                else:
                    result = ['FAILED',0,0]

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
    
def addRunToPlot(run,tag=""):
    from ROOT import TText
    text = TText()
    text.SetNDC()
    text.SetTextSize(0.03)
    text.DrawText( 0.12, 0.85, tag+" Run "+str(run) )
    
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

def rootStyle():
    # make ROOT plots less unpleasant to look at ...
    
    from ROOT import gROOT, gStyle, kWhite, kBlack

    # Start from a plain default
    gROOT.SetStyle("Plain")

    lhcbMarkerType    = 8
    lhcbMarkerSize    = 0.8
    lhcbFont          = 62
    lhcbStatFontSize  = 0.02
    lhcbStatBoxWidth  = 0.12
    lhcbStatBoxHeight = 0.12
    lhcbWidth         = 1
    lhcbTextSize      = 0.05
    lhcbLabelSize     = 0.035
    lhcbAxisLabelSize = 0.035
    lhcbForeColour = kBlack

    gStyle.SetFrameBorderMode(0)
    gStyle.SetPadBorderMode(0)

    # canvas options
    gStyle.SetCanvasBorderSize(0)
    gStyle.SetCanvasBorderMode(0)

    # fonts
    gStyle.SetTextFont(lhcbFont)
    gStyle.SetTextSize(lhcbTextSize)
    gStyle.SetLabelFont(lhcbFont,"x")
    gStyle.SetLabelFont(lhcbFont,"y")
    gStyle.SetLabelFont(lhcbFont,"z")
    gStyle.SetLabelSize(lhcbLabelSize,"x")
    gStyle.SetLabelSize(lhcbLabelSize,"y")
    gStyle.SetLabelSize(lhcbLabelSize,"z")
    gStyle.SetTitleFont(lhcbFont)
    gStyle.SetTitleSize(lhcbAxisLabelSize,"x")
    gStyle.SetTitleSize(lhcbAxisLabelSize,"y")
    gStyle.SetTitleSize(lhcbAxisLabelSize,"z")
    gStyle.SetTitleColor(kWhite)
    gStyle.SetTitleFillColor(kWhite)
    gStyle.SetTitleColor(kBlack)
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleTextColor(kBlack)

    # set title position
    gStyle.SetTitleX(0.15)
    gStyle.SetTitleY(0.97)
    # turn off Title box
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleTextColor(lhcbForeColour)
    gStyle.SetTitleColor(lhcbForeColour)

    # use bold lines and markers
    gStyle.SetLineWidth(lhcbWidth)
    gStyle.SetFrameLineWidth(lhcbWidth)
    gStyle.SetHistLineWidth(lhcbWidth)
    gStyle.SetFuncWidth(lhcbWidth)
    gStyle.SetGridWidth(lhcbWidth)
    gStyle.SetLineStyleString(2,"[12 12]")
    gStyle.SetMarkerStyle(lhcbMarkerType)
    gStyle.SetMarkerSize(lhcbMarkerSize)

    # label offsets
    gStyle.SetLabelOffset(0.015)

    # by default, do not display histogram decorations:
    gStyle.SetOptStat(1111)
    # show probability, parameters and errors
    gStyle.SetOptFit(1011)

    # look of the statistics box:
    gStyle.SetStatBorderSize(1)
    gStyle.SetStatFont(lhcbFont)
    gStyle.SetStatFontSize(lhcbStatFontSize)
    gStyle.SetStatX(0.9)
    gStyle.SetStatY(0.9)
    gStyle.SetStatW(lhcbStatBoxWidth)
    gStyle.SetStatH(lhcbStatBoxHeight)

    # put tick marks on top and RHS of plots
    gStyle.SetPadTickX(1)
    gStyle.SetPadTickY(1)

    # histogram divisions
    gStyle.SetNdivisions(505,"x")
    gStyle.SetNdivisions(510,"y")

    # Force the style
    gROOT.ForceStyle()
