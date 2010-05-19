def Hlt2Monitor ( variable ,  label , center , window, id =None, nbins = 100 ) :
    return "process ( monitor( %s,  Gaudi.Histo1DDef( '%s', %s , %s, %s), '%s' ) ) >> ~EMPTY " % (variable, label, center-window,center+window, nbins, id if id else variable )

def Hlt2MonitorMinMax ( variable ,  label , min , max, id =None, nbins = 100 ) :
    return "process ( monitor( %s,  Gaudi.Histo1DDef( '%s', %s , %s, %s), '%s' ) ) >> ~EMPTY " % (variable, label, min,max, nbins, id if id else variable )
