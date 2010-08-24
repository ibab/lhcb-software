from ROOT import TH1D

"""
Utility functions for HPD analysis
"""

from pyHPDXMLParsingUtils import xmmtopixels, ymmtopixels

def imageOffset( rootfile, histname, errorHistName, hpdcopynr ):
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

        if entries > 0 :
            retVal = (1.0*offset)/(1.0*offsetErr)
            retErr = sqrt(1.0/offsetErr)
        else:
            raise Exception('NoFitInformation')
            #retVal = 0.0
            #retErr = 0.0
    
    return (retVal,retErr)

def imageOffsetX( rootfile, hpdcopynr ):
    """
    Returns the HPD local X-offset in mm
    """
    return imageOffset(rootfile,'dPosXvsCopyNr','dPosXvsCopyNrErr',hpdcopynr)

def imageOffsetY( rootfile, hpdcopynr ):
    """
    Returns the HPD local Y-offset in mm
    """
    return imageOffset(rootfile,'dPosYvsCopyNr','dPosYvsCopyNrErr',hpdcopynr)

def hpdLocalOffset( rootfile, hpdcopynr, fullFit = False ):
    """
    Returns the HPD local offset in mm
    """
    if not fullFit:
        xoffset = imageOffsetX( rootfile, hpdcopynr )
        yoffset = imageOffsetY( rootfile, hpdcopynr )
    else:
        import pyHPDImageFit
        fitR = pyHPDImageFit.fit(rootfile,hpdcopynr,1000)
        if fitR["OK"]:
            xoffset = fitR["XShift"]
            yoffset = fitR["YShift"]
        else:
            xoffset = (0,0)
            yoffset = (0,0)
    return xoffset,yoffset

def hpdCentreInPixels( rootfile, hpdcopynr ):
    """
    Returns the HPD image centre in pixels
    """
    x0, y0  = hpdLocalOffset(rootfile,hpdcopynr)
    pixelsx = xmmtopixels( x0 )
    pixelsy = ymmtopixels( y0 )
    return (pixelsx,pixelsy)
    
def hpdImageRadius( rootfile, hpdcopynr ):
    """
    Returns the HPD photocathode radius in mm
    """
    return imageOffset(rootfile,'RadiusvsCopyNr','RadiusvsCopyNrErr',hpdcopynr)

def hpdImageRadiusInPixels( rootfile, hpdcopynr ):
    """
    Returns the HPD photocathode radius in pixels
    """
    rad = hpdImageRadius( rootfile, hpdcopynr )
    return 2*rad

