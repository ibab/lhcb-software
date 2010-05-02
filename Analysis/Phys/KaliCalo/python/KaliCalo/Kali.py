#!/usr/bin/env python
# ======================================================================
## @file KaliCalo/Kali.py
#  Set of useful utilities & classes
#       for ``iterative pi0'' Ecal calibration
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-17
# ======================================================================
"""
Set of useful utilities & classes
for ``iterative pi0'' Ecal calibration
"""
# ======================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__    = " 2010-03-17 "
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.17 $ "
# ======================================================================
import ROOT
from GaudiPython.Bindings import gbl as cpp

## the same cell 
from KaliCalo.Cells     import  ( CellID    , GlobalRow  , GlobalCol ,
                                  InnerZone , MiddleZone , OuterZone ,
                                  EcalZone  , Zones     ) 
from KaliCalo.FakeCells import SameCell


Counter    = cpp.StatEntity
VE         = cpp.Gaudi.Math.ValueWithError

def _ve_str_ ( self , fmt = '( %.3g +- %.3g )' ) :
    return fmt % ( self.value() , self.error() )

def _ve_abs_ ( self ) :
    return VE ( abs( self.value() ) , self.cov2() )

VE.__str__  = _ve_str_
VE.__repr__ = _ve_str_
VE.__abs__  = _ve_abs_

    
# =============================================================================
## Helper class to hold the histogams associated with the given CellID
class Histos(object):
    """
    Helper class to hold the histogams associated with the given CellID
    
    """
    def __init__ ( self , cellID , background = False ) :
        self._cellID     = cellID
        self._background = background 
        self._histos     = self._make_histos_ ( cellID )
        
        self._counters = [
            Counter () , ## beta*Eprs/Egamma for LG case 
            Counter ()   ## beta*Eprs/Egamma for GG case 
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
        if not self._background : return lst
        
        lst += [ self._make_histo_ ( cellID , '_LL_BG' ) ] ## max(eprs1,eprs2)<...
        lst += [ self._make_histo_ ( cellID , '_LG_BG' ) ] ## eprs1<...,eprs2 >...
        lst += [ self._make_histo_ ( cellID , '_GG_BG' ) ] ## min(eprs1,eprs2)>...
        
        return lst

    ## get the number of histograms 
    def __len__  (self ) : return len( self._histos )
        
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
class HistoMap(object) :
    """
    Helper class to hold the map of { cellID : histos }
    """

    ## constructor 
    def __init__ ( self , background = False , *args ) :
        """
        Constructor, empty histogram map 
        """
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
            self._histos [ cellID ] = Histos ( cellID , self._background )
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
                if 0 != key.find ( pattern )            : continue
                o = dbase[key]
                if not issubclass ( type(o)  , Histos ) : continue
                
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

    ## check the presence of the certain key 
    def has_key ( self , key ) :
        """
        check the presence of the certain key
        """
        return self._lambdas.has_key ( key )
    
    ## Save lambdas to data base 
    def save ( self , dbasename , prefix = '' , **args ) :
        """
        Save histograms to data base 
        """
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename , **args )
        for key in self : 
            pattern = prefix + 'Lambdas '
            dbase [ pattern + str(key) ] = ( key , self [ key ] )
        dbase.close()
        return len ( self )
        
    ## read the histograms from zipped data base 
    def read ( self , dbasename , prefix = ''  , **args ) :
        """
        Read the histograms from zipped data base
        """
        ## reset all histos 
        self._lambdas = {}
        ## 
        import KaliCalo.ZipShelve as ZipShelve
        dbase = ZipShelve.open  ( dbasename , 'r' , **args )
        for key in dbase :
            pattern = prefix + 'Lambdas '
            if 0 != key.find ( pattern )            : continue
            o = dbase[key]
            self._lambdas [ o[0] ] = o[1]

        dbase.close() 
        return len ( self )

    ## Construct the map from the subsequent iterations
    def __imul__    ( self , other ) :
        """
        Construct the map from the subsequent iterations
        """
        for key in self :
            if key in Zones             : continue 
            if not other.has_key( key ) : continue
            lams = self[key]
            if not lams                 : continue 
            lam  = float( lams[-1] ) 
            olams = other[key]
            for ol in olams:
                lams.append ( lam * ol )

        return self
        

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
            if key in Zones : continue 
            lams = self._lambdas[key]
            if 1.0 == lams[ -1 ] : continue 
            _ls [ key ] = self._lambdas[key][-1]
        return _ls
    
    ## get the mean+-rms value for correction coefficients:
    def mean        ( self ) :
        """
        get the mean value for correction coefficients
        Attention: missing keys do nto contribute! 
        """
        if not self : return VE ( 1 , 0 )
        
        cnt = Counter()
        for key in  self._lambdas :
            if key in Zones : continue 
            lam = self._lambdas[key][-1]
            cnt += lam
        return VE ( cnt.flagMean() , cnt.flagRMS()**2 ) 

    ## get the delta of "last" measurement 
    def delta ( self , last = 1 , prev = 1 ) :
        """
        Get the delta of measurements
        """
        cnt = Counter()
        for key in self._lambdas :
            if key in Zones : continue 
            lam = self._lambdas[key]
            if len ( lam ) <  last + prev : continue
            d = float( lam[ -last ] ) / lam[ -(last+prev) ] 
            cnt += ( d - 1.0 ) 
        return cnt 

    ## get list of cells with 'large' corrections 
    def large ( self , delta ) :
        """
        get list of cells with 'large' corrections
        """
        result = []
        for key in self._lambdas :
            if key in Zones : continue 
            lam = self._lambdas[key]
            if len ( lam ) < 2 : continue
            d = abs ( float( lam[-1] ) / lam[-2] - 1.0 )  
            if d > delta : result.append  ( key )
        return result 
        
    ## Get the number of items/keys 
    def __len__     ( self ) :
        """
        Get the number of items/keys :

        >>> len ( m ) 
        
        """
        return len ( self._lambdas )

    ## produce short report 
    def report ( self , values = ( 0.150 , 0.099 , 0.090  ,
                                   0.080 , 0.070 , 0.060  ,
                                   0.050 , 0.040 , 0.030  ,
                                   0.020 , 0.010 , 0.005 ) ) :
        """
        Produce short report
        """
        _m  = self.mean  ()
        _d  = self.delta ()
        print ' Lambdas : ' , _m
        print ' Deltas  : ( %.4f +- %.4f )  %.4f   %.4f ' % (
            _d.flagMean () ,
            _d.flagRMS  () ,
            _d.flagMin  () ,
            _d.flagMax  () )
        for v in values :
            print ' > %3g%% : %d ' % ( v*100 , len ( self.large  ( v ) ) )    

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
