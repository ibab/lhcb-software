from ROOT import TH1D

"""
Utility functions for HPD analysis
"""

from pyHPDXMLParsingUtils import xmmtopixels, ymmtopixels

def imageOffset( rootfile, histname, hpdcopynr, minEntries = 1 ):
    """
    Extracts the offset from the monitoring histogram
    """

    retVal = 0
    retErr = 0

    fullhistname = 'RICH/RichHPDImageSummary/'+histname
    hist = rootfile.Get(fullhistname)
    
    if None == hist:
        
        raise Exception('HistogramMissing')

    else:
        
        nentries  = hist.GetBinContent(2*hpdcopynr+1)
        offset    = hist.GetBinContent(2*hpdcopynr+2)
        offsetErr = hist.GetBinError(2*hpdcopynr+2)

        # Check for min number of entries
        if nentries < minEntries :
            raise Exception('BinHasNoEntries')
        else:
            retVal = (1.0*offset)/(1.0*nentries)
            retErr = (1.0*offsetErr)/(1.0*nentries)
    
    return (retVal,retErr)

def imageOffsetX( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local X-offset in mm
    """
    return imageOffset(rootfile,'dPosXvsCopyNr',hpdcopynr, minEntries)

def imageOffsetY( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local Y-offset in mm
    """
    return imageOffset(rootfile,'dPosYvsCopyNr',hpdcopynr, minEntries)

def hpdLocalOffset( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD local offset in mm
    """
    xoffset  = imageOffsetX( rootfile, hpdcopynr, minEntries )
    yoffset  = imageOffsetY( rootfile, hpdcopynr, minEntries )
    return (xoffset,yoffset)

def hpdCentreInPixels( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD image centre in pixels
    """
    x0, y0  = hpdLocalOffset(rootfile,hpdcopynr,minEntries)
    pixelsx = xmmtopixels( x0 )
    pixelsy = ymmtopixels( y0 )
    return pixelsx, pixelsy
    
def hpdImageRadius( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD photocathode radius in mm
    """
    rad = imageOffset( rootfile, 'RadiusvsCopyNr', hpdcopynr, minEntries )
    return rad

def hpdImageRadiusInPixels( rootfile, hpdcopynr, minEntries = 1 ):
    """
    Returns the HPD photocathode radius in pixels
    """
    rad = hpdImageRadius( rootfile, hpdcopynr, minEntries )
    return 2*rad

