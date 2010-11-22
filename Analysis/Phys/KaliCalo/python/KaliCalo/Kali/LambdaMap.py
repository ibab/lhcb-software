#!/usr/bin/env python
# =============================================================================
# @file LambdaMap.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2010-10-28
# =============================================================================
"""
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"
__date__ = "$Date$"
__version__ = "$Revision$"

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
