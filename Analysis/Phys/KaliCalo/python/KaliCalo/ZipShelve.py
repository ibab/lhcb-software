"""
This is  zip-version of shelve-data base

"""
# =============================================================================
__all__ = (
    'ZipShelf' ,
    'open'      
    )
# =============================================================================

try:
    from cPickle import Pickler, Unpickler, HIGHEST_PROTOCOL
except ImportError:
    from  pickle import Pickler, Unpickler, HIGHEST_PROTOCOL 

try:
    from cStringIO import StringIO
except ImportError:
    from  StringIO import StringIO


import os
import zlib        ## use zlib to compress dbfile
import shelve      ## 
import shutil

# =============================================================================
## Zipped-version of ``shelve''-database     
class ZipShelf(shelve.Shelf):
    """
    Zipped-version of ``shelve''-database     
    """    
    def __init__(
        self                                   ,
        filename                               ,
        flag      = 'c'                        , 
        protocol  = HIGHEST_PROTOCOL           , 
        compress  = zlib.Z_BEST_COMPRESSION    ,
        writeback = False                      ,
        silent    = False                      ) :

        self.__gzip          = False 
        self.__filename      = filename
        self.__remove        = False
        self.__silent        = silent
        self.__opened        = False 

        if not self.__silent :
            print 'ZipShelve: ', filename
            
        if filename.rfind ( '.gz' ) + 3 == len ( filename ) :
            
            if os.path.exists ( filename ) and 'r' == flag :
                ## gunzip into temporary location
                filename_ = self._gunzip ( filename ) 
                if not os.path.exists ( filename_ ) :
                    raise TypeError ( "Unable to gunzip properly:" + filename )
                if not self.__silent : 
                    size1 = os.path.getsize ( filename  ) 
                    size2 = os.path.getsize ( filename_ )
                    print "GZIP uncompression %s: %.1f%%" %  ( filename , (size2*100.0)/size1 ) 
                filename        = filename_ 
                self.__filename = filename_
                self.__remove   = True
            elif os.path.exists ( filename ) and 'r' != flag :
                ## unzip in place
                filename_     = filename[:-3]
                # remove existing file (if needed) 
                if os.path.exists ( filename_ ) : os.remove ( filename_ )
                size1 = os.path.getsize ( filename  ) 
                # gunzip in place 
                self.__in_place_gunzip  ( filename ) 
                ##
                if not os.path.exists ( filename_ ) :
                    raise TypeError ( "Unable to gunzip properly:" + filename )
                if not self.__silent : 
                    size2 = os.path.getsize ( filename_ )
                    print "GZIP uncompression %s: %.1f%%" %  ( filename , (size2*100.0)/size1 )
                filename        = filename_ 
                self.__gzip     = True 
                self.__filename = filename_
                self.__remove   = False
            else : 
                ## 
                filename        = filename[:-3]
                self.__gzip     = True 
                self.__filename = filename

        import anydbm
        shelve.Shelf.__init__ (
            self                                   ,
            anydbm.open ( self.__filename , flag ) ,
            protocol                               ,
            writeback                              )
        
        self.compresslevel = compress
        self.__opened      = True 
        
    ## destructor 
    def __del__ ( self ) :
        """
        Destructor 
        """
        if self.__opened : self.close()  
    
    ## cloze and gzip (if needed)
    def close ( self ) :
        """
        Close the file (and gzip it if required) 
        """
        if not self.__opened : return 
        ##
        shelve.Shelf.close ( self )
        self.__opened = False  
        ##
        if self.__remove and os.path.exists ( self.__filename ) :
            if not self.__silent : print 'REMOVE: ', self.__filename
            os.remove ( self.__filename )
        ##
        if self.__gzip and os.path.exists ( self.__filename ) :
            # get the initial size 
            size1 = os.path.getsize ( self.__filename )
            # gzip the file
            self.__in_place_gzip ( self.__filename ) 
            #
            if not os.path.exists ( self.__filename + '.gz' ) :
                print 'Unable to compress the file ', self.__filename   
            size2 = os.path.getsize( self.__filename + '.gz' )
            if not self.__silent : 
                print 'GZIP compression %s: %.1f%%' % ( self.__filename, (size2*100.0)/size1 )

    ## gzip the file (``in-place'') 
    def __in_place_gzip   ( self , filein ) :
        """
        Gzip the file ``in-place''
        
        It is better to use here ``os.system'' or ``popen''-family,
        but it does not work properly for multiprocessing environemnt
        
        """
        if os.path.exists ( filein + '.gz' ) : os.remove ( filein + '.gz' )
        #
        # gzip the file 
        fileout = self._gzip ( filein )
        # 
        if os.path.exists ( fileout ) :
            # rename the temporary file 
            shutil.move ( fileout , filein + '.gz' )
            #
            import time
            time.sleep( 3 )
            #
            # remove the original
            os.remove   ( filein ) 

    ## gunzip the file (``in-place'') 
    def __in_place_gunzip   ( self , filein ) :
        """
        Gunzip the file ``in-place''
        
        It is better to use here ``os.system'' or ``popen''-family,
        but unfortunately it does not work properly for multithreaded environemnt
        
        """
        #
        filename = filein[:-3]            
        if os.path.exists ( filename ) : os.remove ( filename )
        #
        # gunzip the file 
        fileout = self._gunzip ( filein )
        #        
        if os.path.exists ( fileout ) :
            # rename the temporary file 
            shutil.move ( fileout , filename )
            #
            import time
            time.sleep( 3 )
            #
            # remove the original
            os.remove   ( filein ) 

    ## gzip the file into temporary location, keep original
    def _gzip   ( self , filein ) :
        """
        Gzip the file into temporary location, keep original
        """
        if not os.path.exists  ( filein  ) :
            raise NameError ( "GZIP: non existing file: " + filein )
        #
        fin  =      file ( filein  , 'r' )
        #
        import tempfile 
        fd,fileout = tempfile.mkstemp ( prefix = 'tmp_' , suffix = '_zdb.gz' )
        #
        import gzip 
        fout = gzip.open ( fileout , 'w' )
        #
        try : 
            for all in fin : fout.write ( all )
        finally:
            fout.close()
            fin .close()   
            import time
            time.sleep( 3 ) 
        return fileout
        
    ## gzip the file into temporary location, keep original
    def _gunzip ( self , filein ) :
        """
        Gunzip the file into temporary location, keep original
        """
        if not os.path.exists  ( filein  ) :
            raise NameError ( "GUNZIP: non existing file: " + filein )
        #
        import gzip 
        fin  = gzip.open ( filein  , 'r' )
        #
        import tempfile 
        fd,fileout = tempfile.mkstemp ( prefix = 'tmp_' , suffix = '_zdb' )
        fout = file ( fileout , 'w' )
        #
        try : 
            for all in fin : fout.write ( all )
        finally: 
            fout.close()
            fin .close()
            import time
            time.sleep( 3 ) 
        return fileout
    
# =============================================================================
## ``get-and-uncompress-item'' from dbase 
def _zip_getitem (self, key):
    """
    ``get-and-uncompress-item'' from dbase 
    """
    try:
        value = self.cache[key]
    except KeyError:
        f = StringIO(zlib.decompress(self.dict[key]))
        value = Unpickler(f).load()
        if self.writeback:
            self.cache[key] = value
    return value

# =============================================================================
## ``set-and-compress-item'' to dbase 
def _zip_setitem ( self , key , value ) :
    """
    ``set-and-compress-item'' to dbase 
    """
    if self.writeback:
        self.cache[key] = value
    f = StringIO()
    p = Pickler(f, self._protocol)
    p.dump(value)
    self.dict[key] = zlib.compress( f.getvalue(), self.compresslevel)


ZipShelf.__getitem__ = _zip_getitem
ZipShelf.__setitem__ = _zip_setitem

def open ( filename                                   ,
           flag          = 'c'                        ,
           protocol      = HIGHEST_PROTOCOL           ,
           compresslevel = zlib.Z_BEST_COMPRESSION    , 
           writeback     = False                      ,
           silent        = True                       ) : 
    """
    Open a persistent dictionary for reading and writing.
    
    The filename parameter is the base filename for the underlying
    database.  As a side-effect, an extension may be added to the
    filename and more than one file may be created.  The optional flag
    parameter has the same interpretation as the flag parameter of
    anydbm.open(). The optional protocol parameter specifies the
    version of the pickle protocol (0, 1, or 2).
    
    See the module's __doc__ string for an overview of the interface.
    """
    
    return ZipShelf ( filename      ,
                      flag          ,
                      protocol      ,
                      compresslevel ,
                      writeback     ,
                      silent        )

# =============================================================================
# The END 
# =============================================================================
