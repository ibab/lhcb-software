"""
Utility functions for HPD analysis
"""

from pyHPDXMLParsingUtils import xmmtopixels, ymmtopixels

def imageOffset( rootfile, histname, errorHistName, hpdcopynr, minEntries = 1 ):
    """
    Extracts the offset from the monitoring histogram
    """

    from math import sqrt

    retVal = 0
    retErr = 0

    # Load histograms
    fullhistname = 'RICH/RichHPDImageSummary/'+histname
    hist = rootfile.Get(fullhistname)
    fullerrorhistname = 'RICH/RichHPDImageSummary/'+errorHistName
    errorHist = rootfile.Get(fullerrorhistname)
    entriesHistName = 'RICH/RichHPDImageSummary/entriesvsCopyNr'
    entriesHist     = rootfile.Get(entriesHistName)
  
    if None == hist or None == errorHist or None == entriesHist :

        raise Exception('HistogramMissing')

    else:

        entries   = entriesHist.GetBinContent(hpdcopynr+1)
        offset    = hist.GetBinContent(hpdcopynr+1)
        offsetErr = errorHist.GetBinContent(hpdcopynr+1)

        if entries >= minEntries :
            retVal = (1.0*offset)/(1.0*offsetErr)
            retErr = sqrt(1.0/offsetErr)
        else:
            raise Exception('NoFitInformation')
    
    return (retVal,retErr)

def imageOffsetX( rootfile, hpdcopynr, minEntries ):
    """
    Returns the HPD local X-offset in mm
    """
    return imageOffset(rootfile,'dPosXvsCopyNr','dPosXvsCopyNrErr',hpdcopynr,minEntries)

def imageOffsetY( rootfile, hpdcopynr, minEntries ):
    """
    Returns the HPD local Y-offset in mm
    """
    return imageOffset(rootfile,'dPosYvsCopyNr','dPosYvsCopyNrErr',hpdcopynr,minEntries)

def hpdLocalOffset( rootfile, hpdcopynr, minEntries, fullFit = False ):
    """
    Returns the HPD local offset in mm
    """
    xoffset = (0,0)
    yoffset = (0,0)
    if not fullFit:

        # Get the results from the summary histograms saved in the ROOT file
        xoffset = imageOffsetX( rootfile, hpdcopynr, minEntries )
        yoffset = imageOffsetY( rootfile, hpdcopynr, minEntries )
        
    else:
        # Do a full fit
        
        from GaudiPython import gbl
        from ROOT import TH2D

        # Setup the fit object
        params = gbl.Rich.HPDImage.HPDFit.Params()
        params.cutFraction = 0.1
        params.minBoundary = 3

        # Get the histogram for this HPD
        image = rootfile.Get('RICH/RichHPDImageSummary/Rich_HPD_'+str(hpdcopynr)+'_Image')

        # Do the fit
        fitter = gbl.Rich.HPDImage.HPDFit()
        result = fitter.fit(image,params)

        # Extract the fit results
        if result.OK :
            xoffset = (result.col,result.colErr)
            yoffset = (result.row,result.rowErr)
            
        #import pyHPDImageFit
        #fitR = pyHPDImageFit.fit(rootfile,hpdcopynr,minEntries)
        #if fitR["OK"]:
        #    xoffset = fitR["XShift"]
        #    yoffset = fitR["YShift"]
        #else:
        #    raise Exception('HPDImageFitFailed')
        
    return (xoffset,yoffset)

def hpdCentreInPixels( rootfile, hpdcopynr, fullFit = False ):
    """
    Returns the HPD image centre in pixels
    """
    x0, y0  = hpdLocalOffset(rootfile,hpdcopynr,minEntries,fullFit)
    pixelsx = xmmtopixels( x0 )
    pixelsy = ymmtopixels( y0 )
    return (pixelsx,pixelsy)
    
def hpdImageRadius( rootfile, hpdcopynr, minEntries ):
    """
    Returns the HPD photocathode radius in mm
    """
    return imageOffset(rootfile,'RadiusvsCopyNr','RadiusvsCopyNrErr',hpdcopynr,minEntries)

def hpdImageRadiusInPixels( rootfile, hpdcopynr, minEntries ):
    """
    Returns the HPD photocathode radius in pixels
    """
    rad = hpdImageRadius( rootfile, hpdcopynr, minEntries )
    return 2*rad

