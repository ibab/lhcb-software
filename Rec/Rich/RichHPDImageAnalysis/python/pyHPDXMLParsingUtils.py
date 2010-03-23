import XMLSummaryBase.summary as xmlSummary
from time import ctime  

pixelsize = 0.5
siliconx  = 16.0
silicony  = 16.0

def xpixelstomm( col ):
    return col*pixelsize + 0.5*pixelsize - 0.5*siliconx 

def ypixelstomm( row ):
    return 0.5*silicony - row*pixelsize - 0.5*pixelsize 

def xmmtopixels( x0 ):
    return (x0 + 0.5*siliconx - 0.5*pixelsize)/pixelsize

def ymmtopixels( y0 ):
    return (0.5*silicony - y0 - 0.5*pixelsize)/pixelsize

def countersFromXMLFileList( filelist ):
    """
    Return dictionary of counters from list of XML input files
    """
    vtree = xmlSummary.Merge( filelist )
    return vtree.counter_dict() 


def counterFromDict( countername, counterdict ):
    """
    Return average, error and entries from the dict of counters 
    """
    if countername  in counterdict:
        treevalues = counterdict[countername]
        treearr = treevalues.value()
        if 0 == treearr[1]:
            return (False, 0, 0, 0)
        else:
            return (True, treearr[0],treearr[1], treearr[2])
    else:
        return (False, 0, 0, 0)
       

def hpdLocalOffset( hpd, counterdict ):
    """
    Get the local offset of the HPD in mm
    """
    algstr = 'RichHPDImageSummary/RICH_HPD_' + str(hpd)
    xarr = counterFromDict(algstr+'_XOffset',counterdict)
    yarr = counterFromDict(algstr+'_YOffset',counterdict)
    if xarr[0] and yarr[0]:
        return ( True, xarr[1]/xarr[2],yarr[1]/yarr[2] )
    else:
        return ( False,0,0 )
    
def hpdCentreInPixels( hpd, counterdict ):
    """
    Get the centre of the HPD in pixels from the dictionary 
    """
    isvalid, x0, y0 = hpdLocalOffset( hpd, counterdict )
    if isvalid:
        col0 = xmmtopixels( -x0 )
        row0 = ymmtopixels( -y0 )
        return (isvalid, col0, row0)
    else:
        return (False,0,0)

def hpdRadiusInPixels( hpd, counterdict ):
    """
    Get the HPD PC image in pixels from the dictionary
    """
    algstr = 'RichHPDImageSummary/RICH_HPD_' + str(hpd)
    radarr = counterFromDict(algstr+'_Radius',counterdict)
    if radarr[0]:
        return (radarr[1]/(radarr[2]*pixelsize))
    else:
        return 0
    
    
def intervalOfValidity( counterdict ):
    """
    Get the interval of validity from the dictionary 
    """
    timetree = counterdict['RichHPDImageSummary/RICH_EventTime']
    min = 1E-6*timetree.attrib('min')
    max = 1E-6*timetree.attrib('max')
    return (ctime(minval),ctime(maxval))

def debugPrintImagePosition( hpd, counterdict ):
    """
    Print sensor offset - for debug purposes
    """
    (isvalid, x0,y0) = hpdCentreInPixels( hpd, counterdict )
    if isvalid:
        print '(x0, y0) = (' + str(x0) + ' , ' + str(y0) + ')'
