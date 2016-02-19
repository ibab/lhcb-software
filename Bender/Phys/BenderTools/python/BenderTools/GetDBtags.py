#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
""" Simple function to extract DB-tags from the file

Usage:

    get_dbtags [options] file1 [ file2 [ file3 [ file4 ....'

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    
                                                                 
This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
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
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.GetDBTags' )
else                      : logger = getLogger ( __name__ )
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


# =============================================================================
## guess about the main DDB-tag from the list 
def dddb_tag ( tags ) :
    """Guess about the main DDB-tag from the list
    """
    if isinstance ( tags , str ) : return tags
    #
    for t in tags :
        if 0 == t.find('dddb-') or 0 == t.find('DDDB-') : return t
    #
    return tags[0] 

    
# =============================================================================
def useDBTagsFromData (
    files             ,   ## the file name 
    castor     = True ,   ## use castor/EOS ? 
    grid       = ''   ,   ## use grid?
    daVinci    = None ,   ## davinci instance 
    importOpts = []   ,   ## import options (if any) 
    catalogs   = []   ) : ## XMl  catalogs to use 
    """ Extract the tags from data file  and configure DaVinci
    >>> dv        = DaVinci ( ... )
    >>> datatiles = ...
    >>> from BenderTools.GetDBtags import useDBtagsFromDATA
    >>> useDBtagsFromDATA ( datafiles , castor , grid )    
    """
    
    if isinstance ( files , str ) :  files = [ filenames ]  

    if importOpts :
        from Bender.DataUtils import evtSelInput
        ifiles = evtSelInput ( importOpts )
        files  = files + ifiles

    if not files :
        logger.error('No input files are specified') 
        return {}
    
    tags  = {}
    ifile = 0
    nfile = len(files)
    for f  in files :
        ifile += 1 
        logger.debug ( "Try the file %s   [%d/%d]" %  ( f , ifile , nfile ) ) 
        tags = getDBTags  ( f                       ,
                            castor     = castor     ,
                            grid       = grid       ,
                            importOpts = importOpts ,
                            catalogs   = catalogs   ,
                            debug      = False      )
        if tags : break  
        if   0 == ifile % 100 : 
            logger.error   (" No success in %d/%d trials" % ( ifile , nfile ) )
        elif 0 == ifile %  10 :
            logger.warning (" No success in %d/%d trials" % ( ifile , nfile ) )  

    if not tags : 
        logger.warning ( 'No tags are extracted from %d files ' % nfile )
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
            logger.info ( " CondDB/LocalTags['CONDDB'/'SIMCOND'] : %s " % db.LocalTags['CONDDB'] )
        
        logger.info ( ' DaVinci/CondDBtag : %s ' % daVinci.CondDBtag  )

    return tags


# =============================================================================
## get DB-tags from the data
#  @author Thomas RUF 
#  @author Vanya BELYAEV 
#  @date   2012-10-24  
def _getDBTags_ ( file_name          ,
                  importOpts = []    , 
                  catalogs   = []    , 
                  debug      = False ) :
    
    import os

    logger.info ( "Use the file %s " % file_name )

    from subprocess import Popen, PIPE
    arguments = [ '_dump_db_tags_2_' , file_name ]
    #
    if importOpts : arguments += [ '-i' ] + importOpts
    if catalogs   : arguments += [ '-x' ] + catalogs
    if debug      : arguments += [ '-v' ] 
    #
    logger.verbose( "Use Popen(%s)" % arguments )
    pipe = Popen ( arguments            ,
                   env    = os.environ  ,
                   stdout = PIPE        ,
                   stderr = PIPE        )

    for line in pipe.stderr :
        logger.error ( 'STDERR:%s' % line[-1]  ) 

    ## Format = 'DBTags: { dictionary} '
    key  = 'DBTags:'
    tags = {} 
    for line in pipe.stdout :

        if debug : logger.info ('STDOUT:%s' % line[:-1] ) 

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
## get DB-tags from the data
#  @author Thomas RUF 
#  @author Vanya BELYAEV 
#  @date   2012-10-24  
def getDBTags ( file_name          ,
                castor     = True  ,
                grid       = ''    ,
                importOpts = []    , 
                catalogs   = []    , 
                debug      = False ) :
    
    import os

    file_name = os.path.expandvars ( file_name ) 
    file_name = os.path.expanduser ( file_name ) 
    file_name = os.path.expandvars ( file_name ) 

    tags = {}
    file_name_ext = file_name 
    if not os.path.exists ( file_name ) :
        
        from Bender.DataUtils import extendfile1
        file_name_ext = extendfile1  ( file_name , castor , grid )

    tags = _getDBTags_ ( file_name_ext , importOpts , catalogs , debug )
    
    ## try non-decorated name 
    if not tags and file_name_ext != file_name :
        return _getDBTags_ ( file_name , importOpts , catalogs , debug )

    return tags

# =============================================================================
## get Meta-info from the data
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-06-11  
def _getMetaInfo_ ( file_name          ,
                    importOpts = []    , 
                    catalogs   = []    , 
                    debug      = False ) :

    import os 
    logger.info ( "Use the file %s " % file_name )

    from subprocess import Popen, PIPE
    
    arguments = [ '_dump_meta_info_' , file_name ]
    #
    if importOpts : arguments += [ '-i' ] + importOpts
    if catalogs   : arguments += [ '-x' ] + catalogs
    if debug      : arguments += [ '-v' ] 
    #
    logger.verbose ( "Use Popen(%s)" % arguments )
    pipe = Popen (  arguments           ,
                    env    = os.environ ,
                    stdout = PIPE       ,
                    stderr = PIPE       )
    
    ## Format = 'MetaInfo: { dictionary} '
    key  = 'MetaInfo:'
    info = {} 
    for line in pipe.stdout :
        
        if debug : logger.info ('STDOUT:%s' % line[:-1] ) 
        
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
## get Meta-info from the data
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-06-11  
def getMetaInfo ( file_name          ,
                  castor     = True  ,
                  grid       = ''    ,
                  importOpts = []    , 
                  catalogs   = []    , 
                  debug      = False ) :
    
    import os
    
    file_name = os.path.expandvars ( file_name ) 
    file_name = os.path.expanduser ( file_name ) 
    file_name = os.path.expandvars ( file_name ) 

    metainfo = {}
    file_name_ext = file_name 
    if not os.path.exists ( file_name ) :
        
        from Bender.DataUtils import extendfile1
        file_name_ext = extendfile1  ( file_name , castor , grid )
        
    metainfo = _getMetaInfo_ ( file_name_ext , importOpts , catalogs , debug )
        
    ## try non-decorated name 
    if not metainfo and file_name_ext != file_name :
        return _getMetaInfo_ ( file_name , importOpts , catalogs , debug )

    return metainfo

# =============================================================================
## extract DB-tags form the files
#  @author Vanya BELYAEV 
#  @date   2012-10-24   
def extractTags ( args ) :
    """ Extract DB-tags from the files 
    """
    
    import argparse 
    parser = argparse.ArgumentParser( ) ##**kwargs )
    
    parser.add_argument (
        '-g'                       ,
        '--grid'                   ,
        type    = str              , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over 'castor/eos', but grid proxy is needed) [default : %(default)s]" ,
        default = 'CERN'
        )
    
    parser.add_argument (
        '--no-castor'              ,
        action  = 'store_false'    , 
        dest    = 'Castor'         ,
        help    = "Disable direct access to Castor/EOS storage for LFNs",
        default = True             ## use castor on default 
        )
    
    group = parser.add_mutually_exclusive_group()
    group.add_argument (
        '-v'                           ,
        '--verbose'                    ,
        action  = "store_true"         ,
        dest    = 'Verbose'            ,
        help    = "``Verbose'' processing [defaut : %(default)s ]"  , 
        default = False 
        )
    group.add_argument (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_false"       ,
        dest    = 'Verbose'           ,
        help    = "``Quiet'' processing [defaut : %(default)s ]"  
        )
    
    parser.add_argument (
        "files" ,
        metavar = "FILE"          ,
        nargs   = '*'             ,
        help    = "Input data file(s) to be processed [defaut : %(default)s ]" 
        )
    
    parser.add_argument (
        '-i'                       ,
        '--import'                 ,
        dest    = 'ImportOptions'  ,
        metavar = 'IMPORT'         ,
        nargs   = '*'              ,
        default = []               , 
        help    = """List of files to be used for 'importOptions',
        e.g. input data [default:%(default)s].
        The files are imported at the end of configuration step"""
        )
    
    parser.add_argument (
        '-x'                    ,
        '--xml'                 ,
        dest    = 'XmlCatalogs' ,
        help    = "``XmlCatalogs'' to be transferred to setData-function [default: %(default)s]" ,
        nargs   = '*'           , 
        default = []                
        )
    
    arguments = parser.parse_args ( args )
    
    import logging
    if arguments.Verbose : logging.disable ( logging.DEBUG - 1 )
    else                 : logging.disable ( logging.INFO  - 1 ) 
        
    if arguments.ImportOptions :
        from Bender.DataUtils import evtSelInput
        inputs = evtSelInput ( arguments.ImportOptions )
        if inputs : logger.info('Add %d files from EventSelector.Input' % len(inputs) )  
        arguments.files += inputs

    if not arguments.files :
        raise ArgumentError, "No input files are specifed"
    
    tags = {}

    ifile = 0
    nfile = len(arguments.files) 
    for f in arguments.files : 
        ifile += 1 
        logger.debug ( "Try the file %s   [%d/%d]" %  ( f , ifile , nfile ) ) 
        
        tags = getDBTags ( f                       ,
                           arguments.Castor        ,
                           arguments.Grid          ,
                           arguments.ImportOptions ,
                           arguments.XmlCatalogs   ,                           
                           arguments.Verbose       )
        
        if tags : break
        #
        if   0 == ifile % 100 : 
            logger.error   (" No success in %d/%d trials" % ( ifile , nfile ) )
        elif 0 == ifile %  10 :
            logger.warning (" No success in %d/%d trials" % ( ifile , nfile ) )  

            
    logger.info ( 12*'-'+'+'+57*'-' )
    logger.info ( ' Tags: '         )
    logger.info ( 12*'-'+'+'+57*'-' )
    for k in tags :
        logger.info ( '%11s : %-s ' % ( k , tags[k] ) ) 
    logger.info ( 12*'-'+'+'+57*'-' )
    logger.info ( ' Input files scanned: %d from %s' %  ( ifile , nfile ) )
    logger.info ( ' Last (successful) file "%s"' % f )
    logger.info ( 12*'-'+'+'+57*'-' )

# =============================================================================
## extract MetaInfo from the files
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-06-11   
def extractMetaInfo ( args ) :
    """ Extract MetaInfo-tags from the files 
    """

    import argparse 
    parser = argparse.ArgumentParser( ) ##**kwargs )
    
    parser.add_argument (
        '-g'                       ,
        '--grid'                   ,
        type    = str              , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over 'castor/eos', but grid proxy is needed) [default : %(default)s]" ,
        default = 'CERN'
        )
    
    parser.add_argument (
        '--no-castor'              ,
        action  = 'store_false'    , 
        dest    = 'Castor'         ,
        help    = "Disable direct access to Castor/EOS storage for LFNs",
        default = True             ## use castor on default 
        )
    
    group = parser.add_mutually_exclusive_group()
    group.add_argument (
        '-v'                           ,
        '--verbose'                    ,
        action  = "store_true"         ,
        dest    = 'Verbose'            ,
        help    = "``Verbose'' processing [defaut : %(default)s ]"  , 
        default = False 
        )
    group.add_argument (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_false"       ,
        dest    = 'Verbose'           ,
        help    = "``Quiet'' processing [defaut : %(default)s ]"  
        )
    
    parser.add_argument (
        "files" ,
        metavar = "FILE"          ,
        nargs   = '*'             ,
        help    = "Input data file(s) to be processed [defaut : %(default)s ]" 
        )
    
    parser.add_argument (
        '-i'                       ,
        '--import'                 ,
        dest    = 'ImportOptions'  ,
        metavar = 'IMPORT'         ,
        nargs   = '*'              ,
        default = []               , 
        help    = """List of files to be used for 'importOptions',
        e.g. input data [default:%(default)s].
        The files are imported at the end of configuration step"""
        )
    
    parser.add_argument (
        '-x'                    ,
        '--xml'                 ,
        dest    = 'XmlCatalogs' ,
        help    = "``XmlCatalogs'' to be transferred to setData-function [default: %(default)s]" ,
        nargs   = '*'           , 
        default = []                
        )
    
    arguments = parser.parse_args ( args )

    import logging
    if arguments.Verbose : logging.disable ( logging.DEBUG - 1 )
    else                 : logging.disable ( logging.INFO  - 1 ) 
    
    if arguments.ImportOptions :
        from Bender.DataUtils import evtSelInput
        inputs           = evtSelInput ( arguments.ImportOptions )
        if inputs : logger.info('Add %d files from EventSelector.Input' % len(inputs) )  
        arguments.files += inputs
        
    if not arguments.files :
        raise ArgumentError, "No input files are specifed"
        
    info  = {}
    ifile = 0
    nfile = len(arguments.files) 
    for f in arguments.files  :
        
        ifile +=1 
        logger.debug ( "Try the file %s   [%d/%d]" %  ( f , ifile , nfile ) ) 
        
        info = getMetaInfo ( f                       ,
                             arguments.Castor        ,
                             arguments.Grid          ,
                             arguments.ImportOptions ,
                             arguments.XmlCatalogs   ,                             
                             arguments.Verbose       ) 
        if info : break
        
        if   0 == ifile % 100 : 
            logger.error   (" No success in %d/%d trials" % ( ifile , nfile ) )
        elif 0 == ifile %  10 :
            logger.warning (" No success in %d/%d trials" % ( ifile , nfile ) )  
            

    keys = info.keys()
    keys.sort()
    
    from GaudiPython.Bindings import gbl as cpp
    Time = cpp.Gaudi.Time
    
    logger.info ( 33*'-'+'+'+36*'-' )
    logger.info ( ' MetaInfo: '     ) 
    logger.info ( 33*'-'+'+'+36*'-' )
    for k in keys :
        ##if  k.lower().find('time') + 4 == len(k) and isinstance ( info[k] , (long,int) ) :
        value = info[k]
        if isinstance ( value , dict ) :
            logger.info ( 33*'-'+'+'+36*'-' )
            logger.info ( ' Metainfo: %s' % k[1:] )
            logger.info ( 33*'-'+'+'+36*'-' )
            vkeys = value.keys()
            vkeys.sort()
            fmt = '%-5s %25s'
            for vk in vkeys :
                pk = fmt % ( k[1:] , vk ) 
                logger.info ( '%32s : %-s '      % ( pk , value[vk] ) ) 
        elif  'Time' == k :
            ## @attention: note 1000 here! 
            time   = Time ( 1000 * info [k] )
            logger.info ( '%32s : %-s (%s) ' % ( k , value , time.format(False) ) ) 
        elif 'TCK'  == k or 'Tck' == k :
            logger.info ( '%32s : 0x%08x '   % ( k ,        value ) )
        elif isinstance ( info[k] , set ) : 
            logger.info ( '%32s : %-s '      % ( k , list ( value ) ) ) 
        else : 
            logger.info ( '%32s : %-s '      % ( k ,        value ) ) 
            
    logger.info ( 70*'-')
    logger.info ( ' Input files scanned: %d from %s' %  ( ifile , nfile ) )
    logger.info ( ' Last (successful) file "%s"' % f )
    logger.info ( 70*'-')

# =============================================================================
if '__main__' == __name__ :
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 

    import sys 
    extractTags ( sys.argv[1:] )

# =============================================================================
# The END 
# =============================================================================

