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
import os
        
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
        writeback = False                      ) :

        self.__gzip          = False 
        self.__filename      = filename
        self.__remove        = False
        self.__tmpdir        = None 

        if filename.rfind ( '.gz' ) + 3 == len ( filename ) :
            
            if os.path.exists ( filename ) and 'r' == flag :
                ## copy into temprary location and unsip 
                import tempfile, popen2 
                tmpdir = tempfile.mkdtemp()
                name = filename.split(os.sep)[-1]                
                command = ' cp %s %s && cd %s && gunzip %s '
                command = command % ( filename ,
                                      tmpdir   ,
                                      tmpdir   ,
                                      name     ) 
                cout,cin,cerr = popen2.popen3( command )
                for line in cerr : print ' STDERR: ' , line
                for line in cout : print ' STDOUT: ' , line
                filename        = tmpdir + os.sep + name
                self.__tmpdir   = tmpdir
                self.__filename = filename[:-3]
                self.__remove   = True
            elif os.path.exists ( filename ) and 'r' != flag :
                ## unzip in place 
                size1 = os.path.getsize ( filename ) 
                os.system ( 'gunzip %s ' % filename )                
                size2 = os.path.getsize ( filename[:-3] )
                print "GZIP uncompression %s: %.1f%%" %  ( filename , (size2*100.0)/size1 ) 
                filename        = filename[:-3]
                self.__gzip     = True 
                self.__filename = filename
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

    ## cloze and gzip (if needed)
    def close ( self ) :
        """
        Close the file (and gzip it if required) 
        """
        shelve.Shelf.close ( self )
        ##
        if self.__remove and os.path.exists ( self.__filename ) :
            print 'REMOVE: ', self.__filename 
            os.remove ( self.__filename )
            if self.__tmpdir and os.path.exists ( self.__tmpdir ) :
                print 'REMOVE: ', self.__tmpdir 
                os.rmdir ( self.__tmpdir )
        ##
        if self.__gzip and os.path.exists ( self.__filename ) :
            size1 = os.path.getsize( self.__filename )
            os.system ( 'gzip -9 %s ' % self.__filename )
            size2 = os.path.getsize( self.__filename + '.gz' )
            print 'GZIP compression %s: %.1f%%' % ( self.__filename, (size2*100.0)/size1 ) 
        
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
           writeback     = False                      ) : 
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
                      writeback     )

# =============================================================================
# The END 
# =============================================================================
