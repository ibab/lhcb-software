

def fitSOverB(filename,rad='Rich1Gas',plot='ckResAll'):

    from ROOT import TFile, TF1, TH1, TCanvas

    import RootStyle
    RootStyle.rootStyle(False)

    rootfile = TFile(filename)

    canvas = TCanvas()

    # Get the histogram
    histName = 'RICH/RiCKResLong/'+rad+'/'+plot
    hist = rootfile.Get(histName)

    
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
    hist.Fit(preFitF,"RS0")
    
    # Full Fitting range
    delta = 0.007
    if rad == 'Rich2Gas' : delta = 0.004
    fitMin = preFitF.GetParameter(1) - delta
    fitMax = preFitF.GetParameter(1) + delta
    # Absolute max/min values
    if  rad == 'Rich1Gas' :
        if fitMax >  0.0052 : fitMax =  0.0064
        if fitMin < -0.0052 : fitMin = -0.008
    else:
        if fitMax >  0.0025 : fitMax =  0.0035
        if fitMin < -0.0029 : fitMin = -0.004

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
    hist.Fit(fFitF,"RS0")
    
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
    hist.Fit(fullFitF,"RSE0")
    
    # Fit OK ?
    maxErrorForOK = 1e-4
    fitOK = fullFitF.GetParError(1) < maxErrorForOK
    
    # Draw the histogram
    hist.Draw()
    canvas.Update()
    
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

    intRange = 0.005
    if rad == 'Rich2Gas' : intRange = 0.002

    fullInt = fullFitF.Integral(-intRange,intRange)
    bkgInt  = bkgFunc.Integral(-intRange,intRange)
        
    signal     = (fullInt-bkgInt) / (2.0*intRange)
    background = bkgInt / (2.0*intRange)
    sOverB     =    signal/background

    print "Signal     =", signal
    print "Background =", background
    print "S/B        =", sOverB




    raw_input("Press any key to continue ...")
