# Globals
canvas = None
fitter = None

def hpdfitter():
    from GaudiPython import gbl
    if globals()['fitter'] == None :
        globals()['fitter'] = gbl.Rich.HPDImage.HPDFit()
    return globals()['fitter']

# Get the run number from file name (ugly, but works ...)
def getIntInfo(filename,type):
    run   = 0
    split = ''
    splits = filename.split('_')
    for split in splits:
        s = split.split('-')
        if s[0] == type : run = s[1]
    return int(run.split('.')[0])

def rootCanvas():
    from ROOT import TCanvas
    if globals()["canvas"] == None :
        from ROOT import gROOT
        # No info messages
        gROOT.ProcessLine("gErrorIgnoreLevel = kWarning;")
        # Batch mode (no TCanvas)
        gROOT.SetBatch(True)
        import rootStyle
        rootStyle.applyRootStyle()
        globals()["canvas"] = TCanvas("CKCanvas","CKCanvas",815,750)
    return globals()["canvas"]

def getRootFile(filename):
    import os
    from ROOT import TFile
    file = None
    if filename != "" :
        if os.path.exists(filename):
            file = TFile( filename )
        else:
            print "ERROR :", filename, "does not exist"
    else:
        print "ERROR Accessing ROOT file for job", j.id
    return file

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
    
def fitHPD(filename,copyNumber,fitType='Sobel',clean=True,plotFile=""):

    from GaudiPython import gbl
    from ROOT import TH2D, TEllipse, TText
    import os
    
    # Make a root Canvas
    canvas = rootCanvas()
    canvas.Clear()
    canvas.Divide(2,2)

    # Min number of entries in HPD alignment histogram for update
    minHPDEntries = 10000
    
    # Open the ROOT file
    rootfile = getRootFile(filename)
    if rootfile == None : return

    # Get the histogram for this HPD
    plotname = 'RICH/RichHPDImageSummary/Rich_HPD_'+str(copyNumber)+'_Image'
    image = rootfile.Get(plotname)
    if image == None :
        print "Failed to open", plotname
        return
    if image.GetEntries() < minHPDEntries :
        print "Too few entries"
        return

    # Get the occ plot, for number of events
    occplotname = 'RICH/HPDHitsMoni/PDs/NumHits/CopyNum-'+str(copyNumber)
    occplot = rootfile.Get(occplotname)
    if occplot == None :
        print "Failed to open", occplotname
        return

    nEvents = int(occplot.GetEntries())
   
    canvas.cd(1)
    image.DrawCopy('zcol')
    
    # Cleaner
    cleaner = gbl.Rich.HPDImage.Clean(image,nEvents)
    cleanedImage = cleaner.filter()
    
    canvas.cd(2)
    cleanedImage.Draw('zcol')
    
    # Sobel filter
    sobelFilter = gbl.Rich.HPDImage.SobelFilter(cleanedImage)
    sobelImage = sobelFilter.filter()
    
    # Fit parameters
    params = gbl.Rich.HPDImage.HPDFit.Params()
    params.type           = fitType
    params.cleanHistogram = clean

    # Do the fit
    result = hpdfitter().fit(image,params,nEvents)

    canvas.cd(3)
    sobelImage.Draw('zcol')
    
    if result.OK() :
        print "HPD", copyNumber, "Fit OK : centre", result.col(),",",result.row(), \
              "radius (mm)", result.radInMM()

        # Draw circle
        circle = TEllipse( result.col(), result.row(), result.radInPix(), result.radInPix() )
        circle.SetFillStyle(0)
        circle.SetLineWidth(2)
        circle.Draw()

        # Draw centre point
        centre = TEllipse( result.col(), result.row(), 0.2, 0.2 )
        centre.SetFillStyle(1001)
        centre.SetFillColor(1)
        centre.SetLineWidth(2)
        centre.Draw()

        # Draw boundary pixels
        for pixel in fitter.boundaryPixels():
            p = TEllipse()
            p.SetFillStyle(1001)
            p.SetFillColor(1)
            p.SetLineWidth(2)
            p.DrawEllipse( pixel.col, pixel.row, 0.2, 0.2, 0, 360, 0 )

    canvas.cd(4)
    image.SetTitle(image.GetTitle()+" "+fitType+" fit")
    image.Draw('zcol')
    if result.OK() :
        circle.Draw()
        centre.Draw()

    text = TText()
    text.SetNDC()
    #text.SetTextSize(0.02)
    text.DrawText( 0.3, 0.4, "x " + str(result.x()) )
    text.DrawText( 0.3, 0.5, "y " + str(result.y()) )
    text.DrawText( 0.3, 0.6, "R " + str(result.radInMM()) )
    
    # Print the result
    if plotFile == "" :
        if not os.path.exists('hpdPlots') : os.mkdir('hpdPlots')
        plotname = "hpdPlots/"+fitType+"-HPD"+str(copyNumber)+".png"
        print "Printing to", plotname
        canvas.Print(plotname)
        os.system('display '+plotname+'&')
    else:
        canvas.Print(plotFile)
    
def plotAll(files,minHPD=0,maxHPD=484,fitType="Sobel"):

    # Make a root Canvas
    canvas = rootCanvas()
    canvas.Divide(2,2)

    filenames = rootFileListFromTextFile(files)

    plotname = "hpdPlots/HPDFits_"+fitType+".pdf"
        
    canvas.Print(plotname+"[")

    for filename in filenames :
        for hpd in range(minHPD,maxHPD) :
            fitHPD(filename,hpd,fitType,plotFile=plotname)
        
    canvas.Print(plotname+"]")

    print "Created", plotname
