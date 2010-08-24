from ROOT import TH1D

"""
Utility functions for HPD analysis
"""

from pyHPDXMLParsingUtils import xmmtopixels, ymmtopixels

def imageOffset( rootfile, histname, errorHistName, hpdcopynr, minEntries = 1 ):
    """
    Extracts the offset from the monitoring histogram
    """

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
        
        nentries  = entriesHist.GetBinContent(hpdcopynr)
        offset    = hist.GetBinContent(hpdcopynr)
        offsetErr = errorHist.GetBinContent(hpdcopynr)

        # Check for min number of entries
        if nentries < minEntries :
            raise Exception('BinHasNoEntries')
            #retVal = 0
            #retErr = 0
        else:
            retVal = (1.0*offset)/(1.0*nentries)
            retErr = (1.0*offsetErr)/(1.0*nentries)
    
    return (retVal,retErr)

def imageOffsetX( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local X-offset in mm
    """
    return imageOffset(rootfile,'dPosXvsCopyNr','dPosXErrvsCopyNr',hpdcopynr, minEntries)

def imageOffsetY( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local Y-offset in mm
    """
    return imageOffset(rootfile,'dPosYvsCopyNr','dPosYErrvsCopyNr',hpdcopynr, minEntries)

def hpdLocalOffset( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local offset in mm
    """
    xoffset = imageOffsetX( rootfile, hpdcopynr, minEntries )
    yoffset = imageOffsetY( rootfile, hpdcopynr, minEntries )
    return xoffset,yoffset

def hpdCentreInPixels( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD image centre in pixels
    """
    x0, y0  = hpdLocalOffset(rootfile,hpdcopynr,minEntries)
    pixelsx = xmmtopixels( x0 )
    pixelsy = ymmtopixels( y0 )
    return (pixelsx,pixelsy)
    
def hpdImageRadius( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD photocathode radius in mm
    """
    return imageOffset(rootfile,'RadiusvsCopyNr','RadiusErrvsCopyNr',hpdcopynr,minEntries)

def hpdImageRadiusInPixels( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD photocathode radius in pixels
    """
    rad = hpdImageRadius( rootfile, hpdcopynr, minEntries )
    return 2*rad

