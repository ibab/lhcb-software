#!/usr/bin/env python
# =============================================================================
## @file KaliCalo/Utils.py
#
#  Set of useful utilities for Kali framework
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-23
# =============================================================================
"""
Set of useful utilities for Kali framework
"""
# =============================================================================
__author__   = " Vanya BELYAEV Ivan.Belyaev@itep.ru "
__date__     = " 2010-03-23 "
__version__  = " CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ "
__all__      = (
    "RootFile"          ,
    "POOLCatalogParser"
    )
# =============================================================================
import ROOT 
import os

# =============================================================================
## @class RootFile
#
#  "Smart-Open for ROOT files"
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2010-03-23
# 
class RootFile  ( object ) :
    """
    SmartOpen for ROOT files
    
    @author Vanya BELYAEV Ivan.Belyaev@itep.ru
    @date   2010-03-23
    
    >>> f = RootFile ( name , 'READ' , safe = False )
    
    
    """
        
    ## constructor from filename 
    def __init__ ( self , filename , opts = 'READ' , safe = True )  :
        
        self._remove = False 
        self._file   = None

        filename = filename.replace ('lfn:','LFN:')
        
        ## test various LFN patterns 
        for lfn in ( 'LFN:'        ,
                     '/lhcb/user/' ,
                     '/lhcb/data/' ,
                     '/lhcb/MC/'   ) :
            
            pos = filename.find ( lfn )
            if 0 != pos : continue

            ## 1. try to open using PFN -> LFN converter 
            if self.__open_pfn  ( filename ) : break 
            
            ## 2. try to copy file from GRID 
            if self.__open_grid ( filename ) : break 

        ## open as local file 
        if not self._file :
            try : 
                ## castor?  
                if 0 == filename.find ( '/castor' ) : filename = 'castor:' + filename
                self._file   = ROOT.TFile().Open ( filename , opts  )
                self._remove = False 
            except :
                self._file   = None
                self._remove = False
                
        if safe and not self.isOK () :
            raise NameError ('Unable to open properly %s ' % filename )

    # Open PFN using LFN -> PFN converter 
    def __open_pfn  ( self , filename , catalogue  = None ) :
        """
        Open PFN using LFN -> PFN converter based on local POOL catalogue  
        """
        if not catalogue :
            calalogue = POOLCatalogParser()

        try :
            pfn = catalogue.pfn ( filename )
            if pfn : 
                self._remove = False
                self._file   = ROOT.TFile().Open ( pfn )
            if not self.isOK () :
                raise NameError ('Unable to open properly %s ' % filename )
        except :
            self._remove = False
            self._file   = None 

        return self.isOK() 
                
    ## open file from GRID
    def __open_grid ( self , filename ) :
        """
        Copy the file from the grid into local temporary directory and open it
        """
        name    = filename.split('/')[-1]  ## assume it is LFN

        import os,tempfile
        
        ## build the temporary directory: 
        tmpdir    = tempfile.mkdtemp()
        
        ## create the name for the temporary file
        tmpfile   = tempfile.mktemp( suffix = '.root' )            
        
        ## construct the name for the temporary file in temporary
        localfile = tmpdir + os.sep + name
        
        try :
            
            ## command to copy file from the grid 
            command = ' ( cd %s && dirac-dms-get-file %s ) '
            command = command % ( tmpdir , filename )

            import popen2
            cout, cin, cerr = popen2.popen3 ( command )
            for line in cerr : print ' STDERR: ', line
            
            if not os.path.exists ( localfile ) :
                raise NameError ("Unable to get from GRID " + filename ) 

            ## rename the file 
            os.rename ( locafile , tmpfile )
            
            if not os.path.exists ( tmpfile ) :
                raise NameError ("Unable to get from GRID " + filename ) 
            
            self._remove = True 
            self._file   = ROOT.TFile ( tmpfile , 'READ' )
            
            if not self.isOK () :
                raise NameError ('Unable to open properly %s ' % filename )
            
        except:
            ## clear the temporary file in case of error 
            if os.path.exists ( tmpfile   ) : os.remove ( tmpfile   )
            self._remove = False 
            self._file   = None 
        finally:
            ##  clean the temporary locations 
            if os.path.exists ( localfile ) : os.remove ( localfile )
            if os.path.exists ( tmpdir    ) : os.rmdir  ( tmpdir    ) 

        return self.isOK ()
    
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
        """
        Close Root file 
        """
        fname = None
        if self._file :
            try : 
                fname = self._file.GetName() 
                if self._file and self._file.IsOpen()  :
                    print 'Close ROOT File ', self._file.GetName() 
                    ROOT.gROOT.GetListOfFiles().Remove ( self._file ) 
                    self._file.Close('R')
                del self._file
            except : pass 

        self._file = None
        if fname and self._remove :
            print 'Delete ROOT file ', fname 
            try: 
                os.remove ( fname )
            except :
                pass 

    ## close the file         
    def Close   ( self ) :
        """
        Close the file
        """
        return self.close () 
    ## 
    def __del__ ( self ) : return self.close () 

            

# =============================================================================
## @class   POOLCatalogParser
#  Parser for POOL XML-catalogue.
#  The code is based on the original PoolFileCatalogParser code by Albert Puig
#  @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
#  @author Albert PUIG    Albert.Puig@cern.ch
#  @date 2010-03-23
class POOLCatalogParser (object) :
    """
    Parser for POOL XML-catalogue.
    The code is based on the original PoolFileCatalogParser code by Albert Puig
    
    @author Vanya  BELYAEV Ivan.Belyaev@itep.ru
    @author Albert PUIG    Albert.Puig@cern.ch
    
    """
    ## standard constructor from the catalogue name 
    def __init__ ( self , catalog = './pool_xml_catalog.xml' ) :
        """
        Standard constructor from the catalogue name
        """

        ## guid -> LFN 
        self._lfns = {}
        ## lfn  -> PFNs 
        self._pfns = {}

        try :
            
            from xml.dom import minidom
            
            files  = [] 
            xmldoc = minidom.parse ( catalog )
            main   = xmldoc .childNodes
            for node in main :
                if node.localName == "POOLFILECATALOG" :
                    files += self.__find ( node , 'File' )

            for item in files :
                guid     = item.getAttribute('ID').encode('ascii')
                logicals = self.__find ( item , 'logical' )
                for logical in logicals :
                    lfn = self.__find ( logical , 'lfn' ) [0]
                    lfn = lfn.attributes['name']
                    lfn = lfn.childNodes[0].data.encode('ascii')
                    self._lfns [ guid ] = 'LFN:'+lfn
                
                physicals = self.__find( item , 'physical' )
                for physical in physicals :
                    pfn = self.__find ( physical , 'pfn' ) [0]
                    pfn = pfn.attributes['name']
                    pfn = pfn.childNodes[0].data.encode('ascii')
                
                    lfn = self._lfns[ guid ] 
                    if self._pfns.has_key ( lfn ) : self._pfns[ lfn ].append ( pfn )
                    else                          : self._pfns[ lfn ] = [ pfn ]
                    
        except IOError :
            
            print 'POOLCatalogParse: Unable to parse catalog "%s" ' % catalog
            self._lfns = {} 
            self._pfns = {}
            
            pass
        
        if not self._lfns :
            print 'POOLCatalogParse: Empty list of LFNs : "%s"' % catalog
        if not self._pfns :
            print 'POOLCatalogParse: Empty list of PFNs : "%s"' % catalog


    ## get the list of LFNs 
    def lfns ( self ) :
        """
        Get the list of LFNs
        """
        s = []
        for key in self._lfns : s.append ( self._lfns[key]  )
        s.sort() 
        import sets
        s = sets.Set( s ) 
        return list ( s ) 

    ## get the list of PFNs
    def pfns ( self ) :
        """
        Get the list of PFNs
        """
        s = []
        for key in self._pfns: s += self._pfns[key]  
        s.sort()
        import sets
        s = sets.Set( s ) 
        return list ( s ) 

    ## get PFN for certain LFN
    def pfn ( self , lfn ) :
        """
        Get PFN for certain LFN
        """
        lfn = lfn.replace ('lfn:','LFN:')
        if self._pfns.has_key ( lfn ) : return self._pfns[ lfn ][0]
        if 0 != lfn.find  ( 'LFN:' )  :
            return self.pfn ( 'LFN:' + lfn )
        ##
        return None

    ## get subnodes by name 
    def __find ( self , node , name ) :
        """
        Get subnodes by name
        """
        out = []
        for item in node.childNodes :
            if item.localName == name :
                out.append  ( item ) 
        return out 


# =============================================================================
if '__name__' == __name__ :
    
    ## make printout of the own documentations 
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print ' __all__ : '    ,   __all__  
    print '*'*120  
    
    
# =============================================================================
# The END 
# =============================================================================
