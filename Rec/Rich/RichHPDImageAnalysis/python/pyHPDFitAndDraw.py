# Globals
canvas = None

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
    
def fitHPD(filename,copyNumber,fitType='Sobel',clean=True,plotFile=""):

    from GaudiPython import gbl
    from ROOT import TH2D, TEllipse
    import os
    
    # Make a root Canvas
    canvas = rootCanvas()
    canvas.Clear()
    canvas.Divide(2,2)
    
    # Open the ROOT file
    rootfile = getRootFile(filename)
    if rootfile == None : return

    # Get the histogram for this HPD
    plotname = 'RICH/RichHPDImageSummary/Rich_HPD_'+str(copyNumber)+'_Image'
    image = rootfile.Get(plotname)
    if image == None :
        print "Failed to open", plotname
        return
    
    canvas.cd(1)
    image.Draw('zcol')
    
    # Cleaner
    cleaner = gbl.Rich.HPDImage.Clean(image)
    cleanedImage = cleaner.filter()
    canvas.cd(2)
    cleanedImage.Draw('zcol')
    
    # Sobel filter
    sobelFilter = gbl.Rich.HPDImage.SobelFilter(cleanedImage)
    sobelImage = sobelFilter.filter()
    canvas.cd(3)
    sobelImage.Draw('zcol')
    
    # Fit parameters
    params = gbl.Rich.HPDImage.HPDFit.Params()
    params.type           = fitType
    params.cleanHistogram = clean

    # Do the fit
    fitter = gbl.Rich.HPDImage.HPDFit()
    result = fitter.fit(image,params)

    canvas.cd(4)
    image.SetTitle(image.GetTitle()+" "+fitType+" fit")
    image.Draw('zcol')

    if result.OK() :
        from ROOT import TEllipse
        print "Fit OK : centre",result.col(),",",result.row(),"radius",result.radInPix()

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
            
    # Print the result
    if plotFile == "" :
        if not os.path.exists('hpdPlots') : os.mkdir('hpdPlots')
        plotname = "hpdPlots/"+fitType+"-HPD"+str(copyNumber)+".png"
        print "Printing to", plotname
        canvas.Print(plotname)
        os.system('display '+plotname+'&')
    else:
        canvas.Print(plotFile)
    
def plotAll(filename,fitType='SimpleChi2'):

    # Make a root Canvas
    canvas = rootCanvas()
    canvas.Divide(2,2)
        
    plotname = "hpdPlots/"+fitType+"-allHPDs.pdf"
    canvas.Print(plotname+"[")

    for hpd in range(0,484):
        fitHPD(filename,hpd,fitType,plotFile=plotname)

    canvas.Print(plotname+"]")
    print "Created", plotname
