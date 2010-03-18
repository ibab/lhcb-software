#!/usr/bin/env python
# ======================================================================
## @file KaliCalo.Kali.py
#  Set of useful utilities & classes
#       for ``iterative pi0'' Ecal calibration
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 210-03-17
# ======================================================================
"""
Set of useful utilities & classes
for ``iterative pi0'' Ecal calibration
"""
# ======================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-17 "
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $ " 
# ======================================================================
import ROOT
from GaudiPython.Bindings import gbl as cpp
LHCb         = cpp.LHCb
## decorate CaloCellID class 
import CaloUtils.CellID

CellID     = LHCb.CaloCellID

EcalZone   = CellID ( 'Ecal' , 'PinArea', 0 , 0 )
InnerZone  = CellID ( 'Ecal' , 'Inner'  , 0 , 0 )
MiddleZone = CellID ( 'Ecal' , 'Middle' , 0 , 0 )
OuterZone  = CellID ( 'Ecal' , 'Outer'  , 0 , 0 )

Zones      = (
    OuterZone  ,
    MiddleZone , 
    InnerZone  ,
    EcalZone 
    )

Counter    = cpp.StatEntity
VE         = cpp.Gaudi.Math.ValueWithError

## Helper class to hold the histogams associated with the given CellID
class Histos(object):
    """
    Helper class to hold the histogams associated with the given CellID
    
    """
    def __init__ ( self , cellID ) :
        self._cellID   = cellID 
        self._histos   = self._make_histos_ ( cellID )
        self._counters = [
            Counter () , ## beta*Eprs/(Egamma-beta*Eprs) for LG case 
            Counter ()   ## beta*Eprs/(Egamma-beta*Eprs) for GG case 
            ]
        
    ## get the list of booked histograms 
    def histos  ( self      )  :
        """
        Get the list of booked histograms
        """
        return self._histos
    
    ## get the list of booked counters 
    def counters   ( self  )  :
        """
        get the list of booked counters
        """
        return self._counters 
    
    ## get beta*Eprs/Egamma factors 
    def kappas ( self ) :
        """
        Get beta*Eprs/Egamma factors
        """
        c0 = self._counters[0]  ## beta*Eprs/Egamma for LG case 
        c1 = self._counters[1]  ## beta*Eprs/Egamma for GG case
        
        return ( VE ( c0.flagMean() , c0.flagMeanErr() ** 2 ) ,
                 VE ( c1.flagMean() , c1.flagMeanErr() ** 2 ) ) 
                 
    ## get the booked histogram     
    def histo   ( self , ID )  :
        """
        Get the booked histogram
        """
        return self._histos [ ID ]
    
    ## retrieve CellID for the histogram set 
    def cellID  ( self ) :
        """
        retrieve CellID for the histogram set 
        
        """
        return self._cellID
    
    ## reset all histograms 
    def reset   ( self ) :
        """
        reset all histograms
        """
        for h in self._histos : h.Reset()
        
    ## get the list with numbers of entries for the histograms 
    def entries ( self ) :
        """
        get the list with the number of entries for the histograms 
        """
        return [ int ( h.GetEntries() ) for h in self._histos ] 
    
    ## book all nesesary histograms 
    def _make_histos_ ( self , cellID ) :
        """
        Book all nesesary histograms
        """
        lst  = []
        ## pi0 
        lst += [ self._make_histo_ ( cellID , '_LL'    ) ] ## max(eprs1,eprs2)<...
        lst += [ self._make_histo_ ( cellID , '_LG'    ) ] ## eprs1<...,eprs2 >...
        lst += [ self._make_histo_ ( cellID , '_GG'    ) ] ## min(eprs1,eprs2)>...
        ## Background
        lst += [ self._make_histo_ ( cellID , '_LL_BG' ) ] ## max(eprs1,eprs2)<...
        lst += [ self._make_histo_ ( cellID , '_LG_BG' ) ] ## eprs1<...,eprs2 >...
        lst += [ self._make_histo_ ( cellID , '_GG_BG' ) ] ## min(eprs1,eprs2)>...
        return lst

    ## Book one histogram
    def _make_histo_  ( self , cellID , suffix ) :
        """
        Book one histogram
        """
        id  = str(cellID)
        id  = id.replace ( '(','' )
        id  = id.replace ( ')','' )
        id  = id.replace ( "'",'' )
        id  = id.replace ( ' ','' )
        id  = id.replace ( ',','_')
        id += suffix
        
        h = ROOT.TH1D(
            id                      , ## the name 
            str(cellID)+'__'+suffix , ## the title 
            250                     , ## bins 
            0                       , ## low edge 
            250                     ) ## high edge
        
        return h 

## Helper class to hold the map of { cellID : histos }
class HistoMap(object) :
    """
    Helper class to hold the map of { cellID : histos }
    """

    ## constructor 
    def __init__ ( self , *args ) :
        """
        Constructor, empty histogram map 
        """
        self._histos = {}

    ## Get item from the map. Create the entry for missing key.
    def __getitem__ ( self , cellID ) :
        """
        Get item from the map. Create the entry for missing key.

        >>> m = ... #
        >>> histos = m[ cellID ]
        
        """        
        _hs = self._histos.get( cellID , None )
        if not _hs :
            self._histos [ cellID ] = Histos ( cellID )
            _hs = self._histos[ cellID ]
        return _hs

    def __delitem__ ( self , cellID ) :
        """
        Delete the entry form the map

        >>> m = ... ## get the map
        >>> del m [ cellID ]
        
        """
        if self._histos.has_key ( cellID ) :
            del self._histos[ cellID ]

    ## iteration 
    def __iter__    ( self ) :
        """
        Iteration over the storage

        >>> m = ... # get the storage
        >>> for key in m : print key
        
        """
        for key in self._histos :
            yield key
    
    ## Get the number of items/keys 
    def __len__     ( self ) :
        """
        Get the number of items/keys :

        >>> len ( m ) 
        
        """
        return len ( self._histos )

    ## get all keys 
    def keys    ( self ) :
        """
        Get all keys
        """
        return self._histos.keys()
    
    ## reset all histograms 
    def reset   ( self ) :
        """
        Reset all booked histograms
        """
        for h in self._histos : self._histos[h].reset()
        
    ## get the list with numebr of entries for the histograms 
    def entries ( self ) :
        """
        get the list with the number of entries for the histograms 
        """
        return [ sum ( self._histos[k].entries() ) for k in self._histos ] 

# =============================================================================    
## Helper class to hold the map of { cellID : lambdas }
class LambdaMap(object) :
    """
    Helper class to hold the map of { cellID : lambdas }
    """

    ## constructor 
    def __init__ ( self , *args ) :
        """
        Constructor, empty histogram map 
        """
        self._lambdas = {}

    ## Get item from the map. Create the entry for missing key.
    def __getitem__ ( self , cellID ) :
        """
        Get item from the map. Create the entry for missing key.

        >>> m = ... #
        >>> lambdas = m[ cellID ]
        
        """        
        _ls = self._lambdas.get( cellID , None )
        if not _ls :
            self._lambdas [ cellID ] = [ 1.0 ] 
            _ls = self._lambdas[ cellID ]
        return _ls

    def __delitem__ ( self , cellID ) :
        """
        Delete the entry from the map
        
        >>> m = ... ## get the map
        >>> del m [ cellID ]
        
        """
        if self._lambdas.has_key ( cellID ) :
            del self._lambdas[ cellID ]

    ## iteration 
    def __iter__    ( self ) :
        """
        Iteration over the storage

        >>> m = ... # get the storage
        >>> for key in m : print key
        
        """
        for key in self._lambdas :
            yield key

    ## get the map { cellID : lambda } 
    def lambdas ( self ) :
        """
        Get the map { cellID : lambda }
        """
        _ls = {}
        for key in self._lambdas :
            lams = self._lambdas[key]
            if 1.0 == lams[ -1 ] : continue 
            _ls [ key ] = self._lambdas[key][-1]
        return _ls

    ## Get the number of items/keys 
    def __len__     ( self ) :
        """
        Get the number of items/keys :

        >>> len ( m ) 
        
        """
        return len ( self._lambdas )
    
    
# =============================================================================
# The END 
# =============================================================================
