#!/usr/bin/env python
# =============================================================================
##  This module contains set of simple and useful utilities for booking and  
#   manipulations with Gaudi-AIDA histograms, inspired by Thomas' request
#   @author Vanya BELYAEV ibelyaev@physics.syr.edu
#   @date 2007-08-03
# =============================================================================
"""
This module contains set of simple and useful utilities for booking and
manipulations with Gaudi-AIDA histograms, inspired by Thomas' request

The module contains following public symbols:

  - book       for booking of various 1D,2D&3D-histograms 
  - bookProf   for booking of various 1D&2D-profiles 
  - getAsAIDA  for retrieval of histograms/profiles from HTS in AIDA format
  - getAsROOT  for retrieval of histograms/profiles from HTS in ROOT format 
  - fill       for smart filling of 1D-histogram (AIDA or ROOT)
  - HistoFile  class for storing histograms to a file  
"""
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"
# =============================================================================
__all__    = ( 'book' , 'bookProf' , 'getAsAIDA' , 'getAsROOT' , 'fill', 'HistoFile' )
# =============================================================================

# import core of Gaudi 
import GaudiPython as gaudimodule
# Need ROOT.TFile
from ROOT import TFile
# get the global namespace to get AIDA and aida2root
from GaudiPython.Bindings import gbl
# =============================================================================
## Helper private auxiliary function to get Application Manager 
def _getAppMgr   ( **kwargs  ) :
    """ Helper private auxiliary function to get Application Manager """
    gaudi = kwargs.get ( 'gaudi' , None )
#    if not gaudi : gaudi = gaudimodule.gaudi
    if not gaudi : gaudi = gaudimodule.AppMgr() 
    if not gaudi : raise RuntimeError, 'Unable to get valid ApplicationMgr'
    return gaudi                                               ## RETURN

# =============================================================================    
## Helper private auxiliary function to get iHistogramSvs 
def _getHistoSvc ( **kwargs ) :
    """ Helper private auxiliary function to get iHistogramSvs """ 
    svc = kwargs.get ( 'service' , None )
    if not svc : svc = kwargs.get ( 'svc' , None )
    else       : return svc                                    ## RETURN 
    gaudi = _getAppMgr ( **kwargs ) 
    return gaudi.histsvc ()                                    ## RETURN

# =============================================================================
## Helper private auxiliary function to get iDataSvs 
def _getEvtSvc ( **kwargs ) :
    """ Helper private auxiliary function to get iDataSvs """ 
    svc = kwargs.get ( 'service' , None )
    if not svc : svc = kwargs.get ( 'svc' , None )
    else       : return svc                                    ## RETURN 
    gaudi = _getAppMgr ( **kwargs ) 
    return gaudi.evtsvc()                                      ## RETURN
    
# =============================================================================
## The trivial function to book the various 1D,2D&3D-histograms
def book ( *args , **kwargs ) :
    """
    The trivial function to book the various 1D,2D&3D-histograms

    (1) book the trivial 1D histogram with full path
    
    >>> h1D = book ( 'path/to/my/histo'       , ## path in Histogram Transient Store
                     'cosine of decay angle ' , ## histogram title 
                      100                     , ## number of bins 
                      -1                      , ## low edge
                      100                     ) ## high edge
                      
    (2) book the trivial 1D histogram with directory path and string ID :
    
    >>> h1D = book ( 'path/to/directory'      , ## the path to directory in HTS
                     'H1'                     , ## string histogram identifier
                     'cosine of decay angle ' , ## histogram title 
                      100                     , ## number of bins 
                      -1                      , ## low edge
                      100                     ) ## high edge                     
                          
    (3) book the trivial 1D histogram with directory path and integer ID :
    
    >>> h1D = book ( 'path/to/directory'      , ## the path to directory in HTS
                     124                      , ## integer histogram identifier
                     'cosine of decay angle ' , ## histogram title 
                      100                     , ## number of bins 
                      -1                      , ## low edge
                      100                     ) ## high edge

    (4) book the trivial 2D histogram with full path
    
    >>> h1D = book ( 'path/to/my/histo'       , ## path in Histogram Transient Store
                     'm12**2 versus m13**2'   , ## histogram title 
                      50                      , ## number of X-bins 
                      1.0                     , ## low X-edge
                      4.0                     , ## high X-edge
                      50                      , ## number of Y-bins 
                      1                       , ## low Y-edge
                      2                       ) ## high Y-edge                      

    (5) book the trivial 2D histogram with directory path and literal ID 
    
    >>> h1D = book ( 'path/to/directory'      , ## path in Histogram Transient Store
                     'Dalitz1'                , ## literal histogram identifier 
                     'm12**2 versus m13**2'   , ## histogram title 
                      50                      , ## number of X-bins 
                      1.0                     , ## low X-edge
                      4.0                     , ## high X-edge
                      50                      , ## number of Y-bins 
                      1                       , ## low Y-edge
                      2                       ) ## high Y-edge
                      
    (6) book the trivial 2D histogram with directory path and integer ID 
    
    >>> h1D = book ( 'path/to/directory'      , ## path in Histogram Transient Store
                     854                      , ## integer histogram identifier 
                     'm12**2 versus m13**2'   , ## histogram title 
                      50                      , ## number of X-bins 
                      1.0                     , ## low X-edge
                      4.0                     , ## high X-edge
                      50                      , ## number of Y-bins 
                      1.0                     , ## low Y-edge
                      4.0                     ) ## high Y-edge
                      
    (7) book the trivial 3D histogram with full path
    
    >>> h1D = book ( 'path/to/my/histo'       , ## path in Histogram Transient Store
                     'x vs y vs z '           , ## histogram title 
                      10                      , ## number of X-bins 
                      -1.0                    , ## low X-edge
                      1.0                     , ## high X-edge
                      10                      , ## number of Y-bins 
                      -1.0                    , ## low Y-edge
                      1.0                     , ## high Y-edge
                      10                      , ## number of Z-bins 
                      -1.0                    , ## low Z-edge
                      1.0                     ) ## high Z-edge                  

    (8) book the trivial 3D histogram with directory path and literal ID 
    
    >>> h1D = book ( 'path/to/directory'      , ## path in Histogram Transient Store
                     'xyz'                    , ## literal histogram identifier 
                     'x vs y vs z'            , ## histogram title 
                      10                      , ## number of X-bins 
                      -1.0                    , ## low X-edge
                      1.0                     , ## high X-edge
                      10                      , ## number of Y-bins 
                      -1.0                    , ## low Y-edge
                      1.0                     , ## high Y-edge
                      10                      , ## number of Z-bins 
                      -1.0                    , ## low Z-edge
                      1.0                     ) ## high Z-edge

    (9) book the trivial 3D histogram with directory path and integer ID 
    
    >>> h1D = book ( 'path/to/directory'      , ## path in Histogram Transient Store
                     888                      , ## integer histogram identifier 
                     'x vs y vs z'            , ## histogram title 
                      10                      , ## number of X-bins 
                      -1.0                    , ## low X-edge
                      1.0                     , ## high X-edge
                      10                      , ## number of Y-bins 
                      -1.0                    , ## low Y-edge
                      1.0                     , ## high Y-edge
                      10                      , ## number of Z-bins 
                      -1.0                    , ## low Z-edge
                      1.0                     ) ## high Z-edge

    Many other booking methods are available,
    e.g. for the histograms with non-equidistant bins, see IHistogamSvc::book
    
    """
    svc = _getHistoSvc ( **kwargs )
    if not svc : raise RuntimeError, 'Unable to get valid HistogramService '
    ## book the histogram using the service 
    return svc.book(*args)                                          ## RETURN

book.__doc__ += '\n\n' + '\thelp(gaudimodule.iHistogramSvc.book) : \n\n' \
                + gaudimodule.iHistogramSvc.book       . __doc__ 
book.__doc__ += '\n\n' + '\thelp(IHistogramSvc::book) : \n\n'            \
                + gaudimodule.gbl.IHistogramSvc.book . __doc__

# =============================================================================
## The trivial function to book 1D&2D profile histograms:
def bookProf ( *args , **kwargs ) :
    """
    
    The trivial function to book 1D&2D profile histograms:

    (1) book 1D-profile histogram with full path in Histogram Transient Store:
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           'Energy Correction'   , ## the histogram title
                           100                   , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   ) ## high X-edge
    
    (2) book 1D-profile histogram with directory path and literal ID 
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           'Calibration'         , ## the histogram literal identifier
                           'Energy Correction'   , ## the histogram title
                           100                   , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   ) ## high X-edge
    
    (3) book 1D-profile histogram with directory path and integer  ID 
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           418                   , ## the histogram integer identifier 
                           'Energy Correction'   , ## the histogram title
                           100                   , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   ) ## high X-edge

    (4) book 2D-profile histogram with full path in Histogram Transient Store:
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           'Energy Correction'   , ## the histogram title
                           50                    , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   , ## high X-edge
                           50                    , ## number of Y-bins
                           0.0                   , ## low Y-edge
                           100                   ) ## high Y-edge
    
    (5) book 2D-profile histogram with directory path and literal ID 
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           'Calibration'         , ## the histogram literal identifier
                           'Energy Correction'   , ## the histogram title
                           50                    , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   , ## high X-edge
                           50                    , ## number of Y-bins
                           0.0                   , ## low Y-edge
                           100                   ) ## high Y-edge
    
    (6) book 2D-profile histogram with directory path and integer  ID 
    >>> histo = bookProf ( 'path/to/my/profile'  , ## path in Histogram Transient Store
                           418                   , ## the histogram integer identifier 
                           'Energy Correction'   , ## the histogram title
                           50                    , ## number of X-bins
                           0.0                   , ## low X-edge
                           100                   , ## high X-edge
                           50                    , ## number of Y-bins
                           0.0                   , ## low Y-edge
                           100                   ) ## high Y-edge
    
    Many other booking methods are available,
    e.g. for the histograms with non-equidistant bins, see IHistogamSvc::book
    
    """
    svc = _getHistoSvc ( **kwargs )
    if not svc : raise RuntimeError, 'Unable to get valid HistogramService '
    ## book the histogram using the service 
    return svc.bookProf(*args)                                      ## RETURN


bookProf.__doc__ += '\n\n' + '\thelp(gaudimodule.iHistogramSvc.bookProf) : \n\n' \
                    + gaudimodule.iHistogramSvc.bookProf . __doc__ 
bookProf.__doc__ += '\n\n' + '\thelp(IHistogramSvc::bookProf) : \n\n'             \
                    + gaudimodule.gbl.IHistogramSvc.bookProf . __doc__

# =============================================================================
## The most trivial function to retrieve the histogram from Histogram Transient Store
def getAsAIDA ( path , **kwargs ) :
    """
    
    The most trivial function to retrieve the histogram from Histogram Transient Store
    The histogram is returned by reference to its AIDA-representation (if possible)

    >>> h = getAsAIDA ( 'some/path/to/my/histogram' )
    
    """
    svc = _getHistoSvc ( **kwargs )
    if not svc : raise RuntimeError, 'Unable to get valid HistogramService '
    ## return the histogram 
    return svc.getAsAIDA( path )                                   ## RETURN

getAsAIDA.__doc__ += '\n\n' + '\thelp(gaudimodule.iHistogramSvc.getAsAIDA) : \n\n' \
                     + gaudimodule.iHistogramSvc.getAsAIDA . __doc__ 
getAsAIDA.__doc__ += '\n\n' + '\thelp(gaudimodule.iHistogramSvc.retrieve)  : \n\n' \
                     + gaudimodule.iHistogramSvc.retrieve  . __doc__ 

# =============================================================================
## The most trivial function to retrieve the histogram from Histogram Transient Store
def getAsROOT ( path , **kwargs ) :
    """
    
    The most trivial function to retrieve the histogram from Histogram Transient Store
    The histogram is returned by reference to its underlying native ROOT-representation (if possible)
    
    >>> h = getAsROOT ( 'some/path/to/my/histogram' )
    
    """
    svc = _getHistoSvc ( **kwargs )
    if not svc : raise RuntimeError, 'Unable to get valid HistogramService '
    ## return the histogram 
    return svc.getAsROOT( path )                                   ## RETURN

getAsROOT.__doc__ += '\n\n' + '\thelp(gaudimodule.iHistogramSvc.getAsROOT) : \n\n' \
                     + gaudimodule.iHistogramSvc.getAsROOT . __doc__ 


# =============================================================================
## The function which allows 'the smart fill' of 1D-histogram
def fill ( histo                   ,   ## histogram 
           data                    ,   ## input data 
           fun   = lambda x : x    ,   ## function to be used  
           cut   = lambda x : True ,   ## cut to be applied
           **kwargs                ) : ## optional extra arguments 
    """
    
    The function which allows 'the smart fill' of 1D-histogram

    >>> histo = ...

    The most trivial case, fill with the value 
    >>> fill ( histo , 1.0 ) 

    Fill from any iterable object (sequence)
    >>> fill ( histo , [1,,2,3,4,5,10] )

    Fill from iterable object and apply the function:
    >>> fill ( histo , [1,2,3,4,5] , math.sin )
    
    Use lambda form:
    >>> fill ( histo , [1,2,3,4,5] , lambda x : x*x )

    The same 
    >>> fill ( histo , [1,2,3,4,5] , fun = lambda x : x*x )

    Use internal attributes:
    >>> tracks = evtSvc['Rec/Track/Best']    ## iterable container of tracks
    >>> fill ( histo , tracks , lambda t : t.pt() )

    Apply the predicate: fill only even numbers: 
    >>> fill ( histo , [1,2,3,4,5,6,7] , lambda x : x , lambda y : y%2 )

    The same (omit the trivial function) : 
    >>> fill ( histo , [1,2,3,4,5,6,7] , cut = lambda y : y%2 )

    Apply the predicate: fill only pt for positively charged tracks:
    >>> tracks = evtSvc['Rec/Track/Best']
    >>> fill ( histo , tracks , lambda t : t.pt() , lambda t : 0<t.charge() )

    The same: 
    >>> fill ( histo , tracks ,
               fun = lambda t : t.pt()       ,
               cut = lambda t : 0<t.charge() )

    Ordinary functions are also fine:
    >>> def myfun ( track ) : return sin( track.pt() + track.p() )
    >>> def mycut ( track ) : return track.p() > 100 * GeV 
    >>> fill ( histo , tracks , myfun , mycut )
        
    The 'data' could be the address in TES, in this case the object
    is retrieved from TES and the method is applied to the objects,
    retrieved from TES:
    >>> fill (  histo             , ## the reference to the histogram 
               'Rec/Track/Best'   , ## the location of objects in TES
                lambda t : t.pt() ) ## function to be used for histogram fill
    >>> fill (  histo             , ## the reference to the histogram
               'Rec/Track/Best'   , ## the address of objects in TES
                lambda t : t.pt() , ## the function to be used for histogram fill
                lambda t : t.charge()>0 ) ## the criteria to select tracks 
    
    The arguments 'fun' and 'cut' could be strings, in this case
    they are evaluated by python before usage.
    This option could be often very useful.
    
    """
    
    # if value is a string, try to get the objects from TES
    if type ( data ) == str :
        svc  = _getEvtSvc ( **kwargs ) 
        data = svc[data]
        return fill ( histo , data , fun , cut , **kwargs )
    
    # if the function  is a string: evaluate it!
    if type ( fun  ) == str : fun  = eval ( fun  , globals() )
    
    # if the criterion is a string: evaluate it!
    if type ( cut  ) == str : cut  = eval ( cut  , globals() )

    if not hasattr ( data , '__iter__' ) : data = [ data ]

    if not hasattr ( histo , 'fill' ) and hasattr ( histo , 'Fill' ) :
        setattr ( histo , 'fill' , getattr ( histo , 'Fill' ) )
    
    for item in data :
        if not cut ( item )         : continue             ## CONTINUE
        histo.fill ( fun ( item ) )
        
    return histo                                           ## RETURN


# =============================================================================
# =============================================================================
class HistoFile :
    """
    Class to write histograms to a ROOT file.
    hFile = HistoFile("myFile.root")
    myHisto = ...
    hFile.save(myHisto)
    myHisto0 = ...
    myHisto1 = ...
    myHisto2 = ...
    hFile.save(myHisto0, myHisto1, myHisto2)
    histoList = [h0, h1, h2, h3]
    hFile.save(histoList)
    ...
    hFile.close()
    """
    __author__ = "Juan Palacios juan.palacios@nikhef.nl"
    
    def __init__(self, fileName) :
        self.file = TFile(fileName, "RECREATE")
        self.aida2root = gbl.Gaudi.Utils.Aida2ROOT.aida2root
        self.aidaTypes = [gbl.AIDA.IHistogram1D,
                          gbl.AIDA.IHistogram2D,
                          gbl.AIDA.IHistogram3D,
                          gbl.AIDA.IProfile1D,
                          gbl.AIDA.IProfile2D,
                          gbl.AIDA.IHistogram    ]

    def __convertibleType(self, histo) :
        histoType = type(histo)
        for t in self.aidaTypes :
            if histoType == t : return True
        return False
    
    def save(self, *args) :
        """
        This function stores histograms on the file for future saving.
        It takes an arbitrary number of AIDA or ROOT histograms or
        a list of them.
        """
        if args :
            if type(args[0])==list :
                histoList = args[0]
            else :
                histoList = args
            for h in histoList :
                if self.__convertibleType(h) : h = self.aida2root(h)
                h.Write()
            
    def close(self) :
        self.file.Write()
        self.file.Close()

# =============================================================================
if '__main__' == __name__ :
    import sys 
    print __doc__
    for o in __all__ :
        print o
        print sys.modules[__name__].__dict__[o].__doc__
        
    
# =============================================================================
# The END 
# =============================================================================
