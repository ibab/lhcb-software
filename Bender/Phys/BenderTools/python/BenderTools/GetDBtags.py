#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file 	
#  Simple function to extract DB-tags from the file 
#
# @code
# 
#  Usage:
#
#     get_dbtags [options] file1 [ file2 [ file3 [ file4 ....'
#
#  @endcode
#
#  @author Thomas RUF 
#  @author Vanya BELYAEV 
#  @date   2012-10-24
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
#
# =============================================================================
"""
Simple function to extract DB-tags from the file

Usage:

    get_dbtags [options] file1 [ file2 [ file3 [ file4 ....'

"""
__author__  = "Thomas RUF, Vanya BELYAEV"
__date__    = "2012-10-24"
__version__ = "$Revision$"
# =============================================================================
__all__     = (
    'useDBTagsFromData' , 
    'getDBTags'         ,
    'getMetaInfo'       ,
    'extractTags'       ,
    'extractMetaInfo'       
    )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
def _lfn_pfn_strip_ ( file_name ) :
    #
    #
    if 0 < file_name.find ( ' ' ) :
        p = file_name.find ( 'DATAFILE=' )
        if 0 <= p  :
            p1 = file_name.find ( "'", p  + 9 )
            p2 = file_name.find ( "'", p1 + 1 )
            l = len ( file_name )
            
            if 0 < p1 and p1 < p2 and p1 < l and p2 < l : pass 
            else :
                p1 = file_name.find ( '"' , p  + 9 )
                p2 = file_name.find ( '"' , p1 + 1 )
                
            if 0 < p1 and p1 < p2 and p1 < l and p2 < l :
                file_name = file_name [ p1+1 : p2 ]
                
    for p in ( 'PFN:' , 'LFN:' , 'pfn:' , 'lfn:' ) :
        if 0 == file_name.find ( p ) : file_name = file_name[4:]
      
    p = file_name.find ( '?')
    if 0 < p : file_name = file_name [ : p ]
    
    return file_name


## guess about the main DDB-tag from the list 
def dddb_tag ( tags ) :
    """
    Guess about the main DDB-tag from the list
    """
    if isinstance ( tags , str ) : return tags
    #
    for t in tags :
        if 0 == t.find('dddb-') or 0 == t.find('DDDB-') : return t
    #
    return tags[0] 
    
# =============================================================================
def useDBTagsFromData (
    file_name      ,   ## the file name 
    castor  = True ,   ## use castor/EOS ? 
    grid    = ''   ,   ## use grid?
    daVinci = None ) : 
    """
    Extract the tags from data file  and configure DaVinci

    dv        = DaVinci ( ... )
    datatiles = ...

    from BenderTools.GetDBtags import useDBtagsFromDATA

    useDBtagsFromDATA ( datafiles , castor , grid )
    
    """
    if isinstance ( file_name , (list,tuple) ) and file_name : file_name = file_name[0]
    if not file_name :
        logger.error( ' useDBTagsFromDATA: Invalid file name(s) ')
        return

    tags = getDBTags  ( file_name , castor , grid )

    if not tags : 
        logger.warning ( 'No tags are extracted from the file %s' % file_name )
        return tags  
    #
    logger.info    ( 'Extracted tags from DATA are : %s' % tags         )
    
    if not daVinci :
        from Configurables import DaVinci
        daVinci = DaVinci ()
        
    if tags.has_key ( 'DDDB'    ) and tags ['DDDB'   ] :

        daVinci.DDDBtag   = dddb_tag ( tags ['DDDB'  ] )
        logger.info ( ' DaVinci/DDDBtag   : %s ' % daVinci.DDDBtag    )

        if isinstance ( tags['DDDB'] , list ) and  1 < len ( tags['DDDB'] ) :
            from Configurables import CondDB  
            db = CondDB()
            db.LocalTags['DDDB'] = tags['DDDB']
            logger.info ( " ConDB/LocalTags['DDDB'] : %s " % db.LocalTags['DDDB'] )
            
    if tags.has_key ( 'CONDDB'  ) and tags ['CONDDB' ] :

        if isinstance ( tags ['CONDDB'] , str ) :
            daVinci.CondDBtag = tags ['CONDDB']
        else :
            daVinci.CondDBtag = tags ['CONDDB'][0]
            from Configurables import CondDB  
            db = CondDB()
            db.LocalTags['CONDDB'] = tags['CONDDB']
            logger.info ( " ConDB/LocalTags['CONDDB'] : %s " % db.LocalTags['CONDDB'] )

        logger.info ( ' DaVinci/CondDBtag : %s ' % daVinci.CondDBtag  )
        
    if tags.has_key ( 'SIMCOND' ) and tags ['SIMCOND'] :
        
        if isinstance ( tags ['SIMCOND'] , str ) :
            daVinci.CondDBtag = tags ['SIMCOND']
        else :
            daVinci.CondDBtag = tags ['SIMCOND'][0]
            from Configurables import CondDB  
            db = CondDB()
            db.LocalTags['CONDDB'] = tags['SIMCOND']
            logger.info ( " CondDB/LocalTags['SIMCOND'] : %s " % db.LocalTags['SIMCOND'] )
        
        logger.info ( ' DaVinci/CondDBtag : %s ' % daVinci.CondDBtag  )

    return tags


# =============================================================================
## get DB-tags from the data
#  @author Thomas RUF 
#  @author Vanya BELYAEV 
#  @date   2012-10-24  
def getDBTags ( file_name      ,
                castor = True  ,
                grid   = ''    , 
                debug  = False ) :
    
    import os
    
    file_name = os.path.expandvars ( file_name ) 
    file_name = os.path.expanduser ( file_name ) 
    file_name = os.path.expandvars ( file_name ) 

    if not os.path.exists ( file_name ) :
        
        from Bender.DataUtils import extendfile1
        file_name = extendfile1     ( file_name , castor , grid )

    logger.info ( "Use the file %s " % file_name )

    from subprocess import Popen, PIPE

    serr =  open ( '/dev/null' )
    pipe = Popen ( [ '_dump_db_tags_2_'  , file_name ] ,
                   env    = os.environ ,
                   stdout = PIPE       ,
                   stderr = serr       )
    
    stdout = pipe.stdout 
    ts = {}

    ## Format = 'DBTags: { disctionary} '
    key  = 'DBTags:'
    tags = {} 
    for line in stdout :

        if debug : logger.info ( line ) 

        ## good line? 
        p = line.find ( key )
        if not 0 <= p : continue

        try : 
            dct    = line[ p + len(key) : ]
            value  = eval ( dct )
            if not isinstance ( value , dict ) : continue
            tags   = value
            if not tags : continue 
        except :
            continue 
            
    return tags 


# =============================================================================
## get Meta-info from the data
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-06-11  
def getMetaInfo ( file_name      ,
                  castor = True  ,
                  grid   = ''    , 
                  debug  = False ) :
    
    import os
    
    file_name = os.path.expandvars ( file_name ) 
    file_name = os.path.expanduser ( file_name ) 
    file_name = os.path.expandvars ( file_name ) 

    if not os.path.exists ( file_name ) :
        
        from Bender.DataUtils import extendfile1
        file_name = extendfile1     ( file_name , castor , grid )

    logger.info ( "Use the file %s " % file_name )

    from subprocess import Popen, PIPE
    
    with open ( '/dev/null' ) as serr :
        
        pipe = Popen ( [ '_dump_meta_info_'  , file_name ] ,
                       env    = os.environ ,
                       stdout = PIPE       ,
                       stderr = serr       )
        
        stdout = pipe.stdout
        
        ## Format = 'DBTags: { dictionary} '
        key  = 'MetaInfo:'
        info = {} 
        for line in stdout :

            if debug : logger.info ( line ) 

            ## good line? 
            p = line.find ( key )
            if not 0 <= p : continue

            try : 
                dct    = line[ p + len(key) : ]
                value  = eval ( dct )
                if not isinstance ( value , dict ) : continue
                info   = value
                if not info : continue 
            except :
                continue 
            
    return info
    
# =============================================================================
## extract DB-tags form the files
#  @author Vanya BELYAEV 
#  @date   2012-10-24   
def extractTags ( args ) :
    """
    Extract DB-tags from the files 
    """
    
    from optparse import OptionParser as OptParser
    
    parser = OptParser( usage   = __doc__                 ,
                        version = ' %prog ' + __version__ )
    
    ##
    parser.add_option (
        '-g'                       ,
        '--grid'                   ,
        type    = 'str'            , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over -c, but grid proxy is needed)" ,
        default = ''
        )
    ##
    parser.add_option (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor Grid Storage to access LFN-files" ,
        default = True   
        )
    
    parser.add_option (
        '-v'                           ,
        '--verbose'                    ,
        action  = "store_false"        ,
        dest    = 'Verbose'            ,
        help    = "Verbose processing" ,
        default = False   
        )
    
    ##
    options , arguments = parser.parse_args ( args )
    
    if not arguments : parser.error ( 'No input files are specified' )

    tags = {}
    for f in arguments  :

        logger.info ( "Try the file %s " % f )
        
        tags = getDBTags ( f               ,
                           options.Castor  ,
                           options.Grid    , 
                           options.Verbose ) 
        if tags : break

    print 12*'-'+'+'+77*'-'
    print ' Tags: '
    print 12*'-'+'+'+77*'-'
    for k in tags :
        print '%11s : %-s ' % ( k , tags[k] ) 
    print 12*'-'+'+'+77*'-'


# =============================================================================
## extract MetaInfo from the files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-06-11   
def extractMetaInfo ( args ) :
    """
    Extract MetaInfo-tags from the files 
    """
    
    from optparse import OptionParser as OptParser
    
    parser = OptParser( usage   = __doc__                 ,
                        version = ' %prog ' + __version__ )
    
    ##
    parser.add_option (
        '-g'                       ,
        '--grid'                   ,
        type    = 'str'            , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over -c, but grid proxy is needed)" ,
        default = ''
        )
    ##
    parser.add_option (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor Grid Storage to access LFN-files" ,
        default = True   
        )
    
    parser.add_option (
        '-v'                           ,
        '--verbose'                    ,
        action  = "store_false"        ,
        dest    = 'Verbose'            ,
        help    = "Verbose processing" ,
        default = False   
        )
    
    ##
    options , arguments = parser.parse_args ( args )
    
    if not arguments : parser.error ( 'No input files are specified' )

    info = {}
    for f in arguments  :
        
        logger.info ( "Try the file %s " % f )
        
        info = getMetaInfo ( f               ,
                             options.Castor  ,
                             options.Grid    , 
                             options.Verbose ) 
        if info : break


    
    print 12*'-'+'+'+77*'-'
    print ' MetaInfo: '
    print 12*'-'+'+'+77*'-'
    keys = info.keys()
    keys.sort()
    
    from GaudiPython.Bindings import gbl as cpp
    Time = cpp.Gaudi.Time 
    for k in keys :
        if  0<= k.find('Time') :
            ## @attention: note 1000 here! 
            time   = Time ( 1000 * info [k] )
            print '%11s : %-s (%s) ' % ( k , info[k] , time.format(False) )
        elif 'TCK'  == k or 'Tck' == k :
            print '%11s : 0x%08x '   % ( k ,        info[k]   )            
        elif isinstance ( info[k] , set ) : 
            print '%11s : %-s '      % ( k , list ( info[k] ) ) 
        else : 
            print '%11s : %-s '      % ( k ,        info[k]   )
            
    print 12*'-'+'+'+77*'-'

# =============================================================================
if '__main__' == __name__ :
    
    print 120*'*'
    print __doc__ 
    print ' Author(s)  : ', __author__ 
    print ' Version    : ', __version__
    print ' Date       : ', __date__ 
    print 120*'*'

    import sys 
    extractTags ( sys.argv[1:] )

# =============================================================================
# The END 
# =============================================================================

