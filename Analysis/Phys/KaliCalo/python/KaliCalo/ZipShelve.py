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


import zlib        ## use zlib to compress dbfile
import shelve      ## 

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

        import anydbm
        shelve.Shelf.__init__ (
            self                            ,
            anydbm.open ( filename , flag ) ,
            protocol                        ,
            writeback                       )
        
        self.compresslevel = compress

        ## ``get-and-uncompress-item'' form dbase 
        def __getitem__(self, key):
            """
            ``get-and-uncompress-item'' form dbase 
            """
            try:
                value = self.cache[key]
            except KeyError:
                f = StringIO(zlib.decompress(self.dict[key]))
                value = Unpickler(f).load()
                if self.writeback:
                    self.cache[key] = value
            return value

      ## ``set-and-compress-item'' to dbase 
        def __setitem__(self, key, value):
            """
            ``set-and-compress-item'' to dbase 
            """
            if self.writeback:
                self.cache[key] = value
            f = StringIO()
            p = Pickler(f, self._protocol)
            p.dump(value)
            self.dict[key] = zlib.compress( f.getvalue(), self.compresslevel)

            

def open ( filename                                   ,
           flag          = 'c'                        ,
           protocol      = HIGHEST_PROTOCOL           ,
           compresslevel = zlib.Z_DEFAULT_COMPRESSION , 
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
