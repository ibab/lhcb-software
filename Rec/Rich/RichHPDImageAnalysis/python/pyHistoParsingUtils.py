from ROOT import TH1D

"""
Utility functions for HPD analysis
"""

from pyHPDXMLParsingUtils import xmmtopixels, ymmtopixels

def imageOffset( rootfile, histname, hpdcopynr ):
    """
    Extracts the offset from the monitoring histogram
    """

    retVal = 0;
       
    hist = rootfile.Get('RICH/RichHPDImageSummary/'+histname)
    
    if None == hist:
        
        raise Exception('HistogramMissing')

    else:
        
        nentries = hist.GetBinContent(2*hpdcopynr+1)
        offset   = hist.GetBinContent(2*hpdcopynr+2)

        # Protect against div by zero
        if nentries == 0:
            raise Exception('BinHasNoEntries')
        else:
            retVal = (1.0*offset)/(1.0*nentries)
    
    return retVal

def imageOffsetX( rootfile, hpdcopynr ):
    """
    Returns the HPD local X-offset in mm
    """
    return imageOffset(rootfile,'dPosXvsCopyNr',hpdcopynr)

def imageOffsetY( rootfile, hpdcopynr ):
    """
    Returns the HPD local Y-offset in mm
    """
    return imageOffset(rootfile,'dPosYvsCopyNr',hpdcopynr)

def hpdLocalOffset( rootfile, hpdcopynr ):
    """
    Returns the HPD local offset in mm
    """
    xoffset  = imageOffsetX( rootfile, hpdcopynr )
    yoffset  = imageOffsetY( rootfile, hpdcopynr )
    return (xoffset,yoffset)

def hpdCentreInPixels( rootfile, hpdcopynr ):
    """
    Returns the HPD image centre in pixels
    """
    x0, y0  = hpdLocalOffset(rootfile,hpdcopynr)
    pixelsx = xmmtopixels( x0 )
    pixelsy = ymmtopixels( y0 )
    return pixelsx, pixelsy
    
def hpdImageRadius( rootfile, hpdcopynr ):
    """
    Returns the HPD photocathode radius in mm
    """
    rad = imageOffset( rootfile, 'RadiusvsCopyNr', hpdcopynr )
    return rad

def hpdImageRadiusInPixels( rootfile, hpdcopynr ):
    """
    Returns the HPD photocathode radius in pixels
    """
    rad = hpdImageRadius( rootfile, hpdcopynr )
    return 2*rad

