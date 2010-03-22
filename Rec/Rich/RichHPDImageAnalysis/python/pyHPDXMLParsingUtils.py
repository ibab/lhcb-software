import XMLSummaryBase.summary as xmlSummary
from time import ctime  

pixelsize = 0.5
siliconx  = 16.0
silicony  = 16.0

def xpixelstomm( col ):
    return col*pixelsize + 0.5*pixelsize - 0.5*siliconx 

def ypixelstomm( row ):
    return 0.5*silicony - row*pixelsize - 0.5*pixelsize 

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
    treevalues = counterdict[countername]
    treearr = treevalues.value()
    if None == treevalues:
        return (False, 0, 0, 0)
    elif 0 == treearr[1]:
        return (False, 0, 0, 0)
    return (True, treearr[0],treearr[1], treearr[2])


def hpdCentreInPixels( hpd, counterdict ):
    """
    Get the centre of the HPD in pixels from the dictionary 
    """
    algstr = 'RichHPDImageSummary/RICH_HPD_' + str(hpd)
    colarr = counterFromDict(algstr+'_AvCol',counterdict)
    rowarr = counterFromDict(algstr+'_AvRow',counterdict)
    if colarr[0] and rowarr[0]:
        return (True,colarr[1]/colarr[2],rowarr[1]/rowarr[2])
    else:
        return (False,0,0)

def hpdRadiusInPixels( hpd, counterdict ):
    """
    Get the HPD PC image in pixels from the dictionary
    """
    algstr = 'RichHPDImageSummary/RICH_HPD_' + str(hpd)
    radarr = counterFromDict(algstr+'_AvRad',counterdict)
    if radarr[0]:
        return (radarr[1]/radarr[2])
    else:
        return 0
    
def hpdLocalOffset( hpd, counterdict ):
    """
    Get the local offset of the HPD in mm
    """
    isvalid, col0, row0 = hpdCentreInPixels( hpd, counterdict )
    if not isvalid:
        return (isvalid, col0, row0) 
    else:
        x0 = xpixelstomm(col0)
        y0 = ypixelstomm(row0)
        return (isvalid, -x0, -y0)
    
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
