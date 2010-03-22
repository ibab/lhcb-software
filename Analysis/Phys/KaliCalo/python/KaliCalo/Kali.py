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
__version__ = " CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $ " 
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

# =============================================================================
## Helper class to hold the histogams associated with the given CellID
class Histos(object):
    """
    Helper class to hold the histogams associated with the given CellID
    
    """
    def __init__ ( self , cellID ) :
        self._cellID   = cellID 
        self._histos   = self._make_histos_ ( cellID )
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
        if self.cellID() != other.cellID() :
            raise TypeError(" *= Mismatch in CellID"   )
        if len ( self.histos   () ) != len ( other.histos   () ) :
            raise TypeError(" *= Mismatch in Histos"   )
        if len ( self.counters () ) != len ( other.counters () ) :
            raise TypeError(" *= Mismatch in Counters" )
        ## add histograms
        for i in range ( 0 , len ( self.histos() ) ) :
            self._histos[ i ].Add ( other._histos[ i ] )
        for i in range ( 0 , len ( self.counters() ) ) :
            self._counters[ i ] += ( other._counters [ i ] )

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

        h.SetDirectory(0)
        return h 

## ============================================================================
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

    ## Save histograms to data base 
    def save ( self , dbasename , prefix = '' ) :
        """
        Save histograms to data base 
        """
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename )
        for key in self : 
            pattern = prefix + 'Histo '
            dbase [ pattern + str(key) ] = self [ key ]
        dbase.close() 

        
    ## update the histograms form data base 
    def updateFromDB ( self , dbasename , prefix = '' ) :
        """
        Update the histograms form data base
        """
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename , 'r' )
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
        return len ( self )
        
        
    ## read the histograms from zipped data base 
    def read ( self , dbasename , prefix = '' ) :
        """
        Read the histograms from zipped data base
        """
        ## reset all histos 
        self._histos = {}
        ## 
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename , 'r' )
        for key in dbase :
            pattern = prefix + 'Histo '
            if 0 != key.find ( pattern )            : continue
            o = dbase[key]
            if not issubclass ( type(o)  , Histos ) : continue
            self._histos [ o.cellID() ] = o

        dbase.close() 
        return len ( self )
    
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
        
        for key in other._histos :
            
            if not self._histos.has_key ( key ) :
                self._histos [ key ]  = other._histos[ key ]
            else :
                self._histos [ key ] += other._histos[ key ]

    
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

    ## Save lambdas to data base 
    def save ( self , dbasename , prefix = '' ) :
        """
        Save histograms to data base 
        """
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename )
        for key in self : 
            pattern = prefix + 'Lambdas '
            dbase [ pattern + str(key) ] = ( key , self [ key ] )
        dbase.close()
        
    ## read the histograms from zipped data base 
    def read ( self , dbasename , prefix = '' ) :
        """
        Read the histograms from zipped data base
        """
        ## reset all histos 
        self._lambdas = {}
        ## 
        import KaliCalo.ZipShelve as ZipShelve 
        dbase = ZipShelve.open  ( dbasename , 'r' )
        for key in dbase :
            pattern = prefix + 'Lambdas '
            if 0 != key.find ( pattern )            : continue
            o = dbase[key]
            self._lambdas [ o[0] ] = o[1]

        dbase.close() 
        return len ( self )

    
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
## @class RootFile
#  "SmartOpen for ROOT files"
class RootFile  ( object ) :

    ## constructor from filename 
    def __init__ ( self , filename , opts = 'READ' , safe = True )  :
        
        self._file   = None
        self._remove = False 
        
        ## test various LFN patterns 
        for lfn in ( 'LFN:'        ,
                     '/lhcb/user/' ,
                     '/lhcb/data/' ,
                     '/lhcb/MC/'   ) :

            print ' I am here 1 '
            pos = filename.find ( lfn )
            if 0 != pos : continue

            print ' I am here 2 '
            
            import os
            name = filename.split('/')[-1]
            try :
                os.remove ( name )
            except :
                pass

            print ' I am here 3 '
            
            try :
                print ' I am here 3.1 '
                import DIRAC
                print ' I am here 3.2 '
                from DIRAC.Interfaces.API.Dirac import Dirac
                print ' I am here 3.3 '
            except ImportError :
                print 'Unable to import/use DIRAC'
                continue
            
            print ' I am here 4 '
            
            import tempfile
            tmpdir = tempfile.mkdtemp()
            
            print ' I am here 4.1'
            
            dirac = Dirac()
            
            print ' I am here 4.5 '
            
            result = dirac.getFile (
                lfn         = filename ,
                destDir     = tmpdir   ,
                printOutput = True     )
            
            print ' I am here 5 '
            
            if not result['OK'] :
                os.rmdir ( tmpdir )
                raise NameError ( "Unable to pick up LFN '%s'" % filename )
            
            print ' I am here 6 '

            tmpfil = tempfile.mktemp( suffix = '.root' )            
            os.rename ( tmpdir + os.sep + name , tmpfil )
            os.rmdir  ( tmpdir )
            
            print ' I am here 7 '
            
            self._file = ROOT.TFile( tmpfil , opts )
            self._remove = True
            
            print ' I am here 8 '

            break  

            
        if not self._file : 
            ## castor?  
            if 0 == filename.find ( '/castor' ) :
                filename = 'castor:' + filename
            self._file   = ROOT.TFile().Open ( filename , opts  )
            self._remove = False 
                                             
        if safe and not self.isOK () :
            raise NameError('Unable to open properly %s ' % filename )
            

    ## ok ?
    def isOK ( self ) :
        """
        Ok?
        """
        return True if self._file and self._file.IsOpen() else False
    
    ## get something from the file 
    def Get    ( self , what ) :
        """
        Get something from the file
        """
        if not self.isOK() : return None 
        return self._file.Get ( what )

    ## get the name 
    def GetName ( self ) :
        """
        get the name
        """
        if not self.isOK() : return None 
        return self._file.GetName() 
        
    ## close root file 
    def close ( self ) :

        fname = None
        if self._file :
            try : 
                fname = self._file.GetName() 
                if self._file and self._file.IsOpen()  :
                    print 'Close ROOT File '
                    ROOT.gROOT.GetListOfFiles().Remove ( self._file ) 
                    self._file.Close('R')
                del self._file
            except : pass 

        self._file = None
        if fname and self._remove :
            import os
            try :
                print 'Delete ROOT file '
                os.remove ( fname )
            except:
                pass

    def Close   ( self ) : return self.close () 
    ## 
    def __del__ ( self ) : return self.close () 

            
            
            
# =============================================================================
# The END 
# =============================================================================
