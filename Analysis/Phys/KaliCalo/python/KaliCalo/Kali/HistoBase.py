#!/usr/bin/env python
# =============================================================================
# @file Histos.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2010-10-28
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

# =============================================================================
## Helper class to hold the histogams associated with the given CellID
class HistosBase(object):
    """
    Helper class to hold the histogams associated with the given CellID
    
    """
    def __init__ ( self , cellID , histo_suffixes, nCounters ) :
        self._cellID     = cellID
        self._histos     = self._make_histos_ ( cellID , histo_suffixes )
        
        self._counters = [ Counter() for i in range( nCounters ) ]
        
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
        return tuple( [ VE ( counter.flagMean() , counter.flagMeanErr() ** 2 )
                        for counter in self._counters ] )
                 
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

    ## add another set of histograms 
    def __iadd__  ( self , other ) :
        if self.cellID().index() != other.cellID().index() :
            print self.cellID() , other.cellID()
            raise TypeError(" *= Mismatch in CellID"   )
        len_h = min ( len( self.histos   () ) , len ( other.histos   () ) ) 
        len_c = min ( len( self.counters () ) , len ( other.counters () ) ) 
        ## add histograms
        for i in range ( 0 , len_h ) :
            self._histos[ i ].Add ( other._histos[ i ] )
        for i in range ( 0 , len_c ) :
            self._counters[ i ] += ( other._counters [ i ] )

        return self 

    ## get the histogram by index 
    def __getitem__ ( self , index ) :
        """
        get the histogram by index
        """
        return self._histos[ index ]
    
    ## book all nesesary histograms 
    def _make_histos_ ( self , cellID , histo_suffixes ) :
        """
        Book all nesesary histograms
        """
        lst  = []
        for suffix in histo_suffixes:
            if suffix:
              suffix = "_%s" % suffix
            lst += [ self._make_histo_ ( cellID , suffix    ) ]        
        return lst

    ## get the number of histograms 
    def __len__  (self ) :
        return len( self._histos )
        
    ## Book one histogram
    def _make_histo_  ( self , cellID , suffix ) :
        """
        Book one histogram
        """
        id  = str(cellID)
        id  = id.replace ( '(','' )
        id  = id.replace ( ')','' )
        id  = id.replace ( "'",'' )
        id  = id.replace ( '"','' )
        id  = id.replace ( ' ','' )
        id  = id.replace ( ',','_')
        id += suffix
        
        ## h = ROOT.TH1D(
        h = ROOT.TH1F (
            id                      , ## the name 
            str(cellID)+'__'+suffix , ## the title 
            250                     , ## bins 
            0                       , ## low edge 
            250                     ) ## high edge

        h.SetDirectory(0)
        return h 

## ============================================================================
## Helper class to hold the map of { cellID : histos }
class HistoMapBase(object) :
    """
    Helper class to hold the map of { cellID : histos }
    """

    ## constructor 
    def __init__ ( self , histoClass , background = False , *args ) :
        """
        Constructor, empty histogram map 
        """
        self._histoClass = histoClass
        self._histos     = {}
        self._background = background 
        
    ## Get item from the map. Create the entry for missing key.
    def __getitem__ ( self , cellID ) :
        """
        Get item from the map. Create the entry for missing key.

        >>> m = ... #
        >>> histos = m[ cellID ]
        
        """        
        _hs = self._histos.get( cellID , None )
        if not _hs :
            self._histos [ cellID ] = self._histoClass ( cellID , self._background )
            _hs = self._histos[ cellID ]
        return _hs

    ## insert the histos 
    def insert ( self , histos ) :
        """
        insert the histos
        """
        self._histos[ histos.cellID() ] = histos 
            
        
    ## Save histograms to data base 
    def save ( self , dbasename , prefix = '' , **args ) :
        """
        Save histograms to data base 
        """
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename , **args )
        for key in self : 
            pattern = prefix + 'Histo '
            dbase [ pattern + str(key) ] = self [ key ]
        dbase.close()
        ## 
        return len ( self ) , self.entries() 
        
    ## update the histograms form database/databases
    def updateFromDB ( self , dbasenames , prefix = '' , **args ) :
        """
        Update the histograms from (zipped) data base
        """
        if issubclass ( type ( dbasenames ) , str ) :
            dbasenames = [ dbasenames ]
            
        import KaliCalo.ZipShelve as ZipShelve

        for dbasename in dbasenames :
            dbase = ZipShelve.open  ( dbasename , 'r' , **args )                    
            for key in dbase :
                pattern = prefix + 'Histo '
                if 0 != key.find ( pattern )                : continue
                o = dbase[key]
                if not issubclass ( type(o)  , HistosBase ) : continue
                
                if self._histos.has_key ( o.cellID() ) : 
                    self._histos [ o.cellID() ] += o
                else :                 
                    self._histos [ o.cellID() ]  = o
            dbase.close()

        return len ( self ) , self.entries() 
        
    ## read the histograms from zipped data base 
    def read ( self , dbases , prefix = '' ) :
        """
        Read the histograms from zipped data base(s)
        """
        ## reset all histos 
        self._histos = {}
        ##
        return self.updateFromDB ( dbases , prefix )
    
    ## delete the entry form the map
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

    ## update with another histogram set
    def __iadd__ ( self , other ) :
        """
        Update with another histogram set
        """
        for key in other._histos :
            
            if not self._histos.has_key ( key ) :
                self._histos [ key ]  = other._histos[ key ]
            else :
                self._histos [ key ] += other._histos[ key ]
                
        return self 
    
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
        
    ## get the total number of entries for the histograms 
    def entries ( self ) :
        """
        get the total number of entries for the histograms 
        """
        lst =  [ sum ( self._histos[k].entries() ) for k in self._histos ]
        return sum ( lst )

    ## Get the dictionary of histos
    def get ( self , func ) :
        """
        Get the dictionary of histos
        """
        _entries = {}
        for key in self :
            if func ( self[key] ) :
                _entries [ key ] = self[key]
        return _entries
    
    ## split histograms into groups (e.g. for parallel processing)
    def split ( self , num = 60 ) :
         """
         split histograms into groups (e.g. for parallel processing)
         """
         if num > len ( self ) : return self.split ( int(num/2) )
         if 0 >= num           : num = 1    
         #
         result = []
         group  = []
         #
         keys = self.keys ()
         keys.sort()
         for key in keys :
             if len ( group ) == num :
                 result.append ( ( group , len ( result ) ) )
                 group = []
             group += [ self[key] ]
             
         if group : result.append ( ( group , len ( result ) ) )
         
         if 20 < len( self ) and len ( result ) < 10 and 2 < num : 
             return self.split ( int ( num / 2 ) )
             
         ## require at least two groups 
         if 0 < len ( self ) and len ( result ) < 2 and 2 < num :
             return self.split ( int ( num / 2 ) )
         
         return result

# =============================================================================
if '__main__' == __name__ :

   ## make printout of the own documentations 
   print '*'*120
   print                      __doc__
   print ' Author  : %s ' %   __author__    
   print ' Version : %s ' %   __version__
   print ' Date    : %s ' %   __date__
   print ' dir(%s) : %s ' % ( __name__    , dir() )
   print '*'*120  


# =============================================================================
# The END 
# =============================================================================
