#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file BenderTools/Parser.py
#
#  Trivial Parser for Bender-based scripts
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  <b>``C++ ToolKit for Smart and Friendly Physics Analysis''</b>
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software''
#
#
#  @date   2010-09-10
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Trivial Parser for Bender-based scripts

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

By usage of this code one clearly states the disagreement with the smear campaign of Dr.O.Callot et al.:

   ``No Vanya's lines are allowed in LHCb/Gaudi software''


                  $Revision$
Last modification $Date$
               by $Author$

"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@nikhef.nl'
__date__    = "2010-09-10"
__version__ = '$Revision$'
__all__     = ( 'makeParser' ,
                'dataType'   ,
                'theYear'    ,
                'fileList'   )
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'BenderTools.Parser' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
## create the parser
def makeParser ( usage = None ,
                 vers  = None ) :
    """
    Create the parser 
    """
    #
    if not usage : usage = __usage__
    if not vers  : vers  = __version__
    #
    from optparse import OptionParser as OptParser
    parser = OptParser( usage   = usage            ,
                        version = ' %prog ' + vers )
    
    ##
    parser.add_option (
        '-t'                  ,
        '--datatype'          ,
        dest    = 'DataType'  ,
        type    = 'str'       , 
        help    = "``DataType''    attribute to be specified for DaVinci [default : %default]" ,
        default = '2012'  
        )
    ## 
    parser.add_option (
        '-s'                          ,
        '--simulation'                ,
        action  = "store_true"        ,
        dest    = 'Simulation'        ,
        help    = "``Simulation''  flag to be propagated to DaVinci" ,
        default = False   
        )
    ## 
    parser.add_option (
        '-g'                       ,
        '--grid'                   ,
        type    = 'str'            , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over -c, but grid proxy is needed)" ,
        default = 'CERN'
        )
    ## 
    parser.add_option (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor/EOS Grid Storage to access LFN-files" ,
        default = True   
        )
    ##
    parser.add_option (
        '-p'                          ,
        '--print'                     ,
        type    = 'int'               , 
        dest    = 'OutputLevel'       ,
        help    = "``OutputLevel'' attribute for ApplicationMgr/MessageSvc [default : %default]" ,
        default = 3                  
        )
    ## 
    parser.add_option (
        '-m'                          ,
        '--micro'                     ,
        action  = "store_true"        ,
        dest    = 'MicroDST'          ,
        help    = "MicroDST format?"  ,
        default = False   
        )
    ##
    parser.add_option (
        '-x'                           ,
        '--xml'                        ,
        dest    = 'XmlCatalogue'       ,
        help    = "``XmlCatalog'' to be transferred to setData-function [default : %default]" ,
        default = ''                  
        )
    ## 
    parser.add_option (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_true"        ,
        dest    = 'Quiet'             ,
        help    = "``Quiet'' processing"  ,
        default = False   
        )
    ## 
    parser.add_option (
        '-r'                     ,
        '--root'                 ,
        type    = 'str'          ,
        dest    = 'RootInTES'    ,
        help    = 'Root-In-TES'  ,
        default = ''           
        )
    ##
    parser.add_option (
        '-u'                       ,
        '--useoracle'              ,
        action  = "store_true"     ,
        dest    = 'UseOracle'      ,
        help    = "Use Oracle-DB"  ,
        default = False   
        )
    ##
    parser.add_option (
        '-l'                       ,
        '--lumi'                   ,
        action  = "store_true"     ,
        dest    = 'Lumi'           ,
        help    = "Use Lumi?"      ,
        default = False   
        )
    ##
    parser.add_option (
        '-k'                       ,
        '--klist'                  ,
        dest    = 'FileList'       ,
        type    = 'str'            ,
        default = ''               ,
        help    = "A file with list of input file names"
        )
    ##
    parser.add_option (
        '-i'                       ,
        '--import'                 ,
        dest    = 'ImportOptions'  ,
        type    = 'str'            ,
        default = ''               ,
        help    = "A file to be used for 'importOptions'"
        )
    ##
    return parser

## create the parser
def makeArgParser ( usage = None ,
                 vers  = None ) :
    """
    Create the parser 
    """
    #
    if not usage : usage = __usage__
    if not vers  : vers  = __version__
    #
    from argparse import ArgumentParser as ArgParser
    parser = ArgParser( usage   = usage )

    ## version: 
    parser.add_argument ( '--version' , action='version', version = vers  ) 
    ##
    parser.add_argument (
        '-t'                  ,
        '--datatype'          ,
        dest    = 'DataType'  ,
        type    =  str        , 
        help    = "``DataType''    attribute to be specified for DaVinci [default : %(default)s]" ,
        default = '2012'  
        )
    ## 
    parser.add_argument (
        '-s'                          ,
        '--simulation'                ,
        action  = "store_true"        ,
        dest    = 'Simulation'        ,
        help    = "``Simulation''  flag to be propagated to DaVinci" ,
        default = False   
        )
    ## 
    parser.add_argument (
        '-g'                       ,
        '--grid'                   ,
        type    = str              , 
        dest    = 'Grid'           ,
        help    = "Grid-site to access LFN-files (has precedence over -c, but grid proxy is needed)" ,
        default = 'CERN'
        )
    ## 
    parser.add_argument (
        '-c'                          ,
        '--castor'                    ,
        action  = "store_true"        ,
        dest    = 'Castor'            ,
        help    = "Enable direct access to Castor/EOS Grid Storage to access LFN-files" ,
        default = True   
        )
    ##
    parser.add_argument (
        '-p'                          ,
        '--print'                     ,
        type    = int                 , 
        dest    = 'OutputLevel'       ,
        help    = "``OutputLevel'' attribute for ApplicationMgr/MessageSvc [default : %(default)s]" ,
        default = 3                  
        )
    ## 
    parser.add_argument (
        '-m'                          ,
        '--micro'                     ,
        action  = "store_true"        ,
        dest    = 'MicroDST'          ,
        help    = "MicroDST format?"  ,
        default = False   
        )
    ##
    parser.add_argument (
        '-x'                           ,
        '--xml'                        ,
        dest    = 'XmlCatalogue'       ,
        help    = "``XmlCatalog'' to be transferred to setData-function [default : %(default)s]" ,
        default = ''                  
        )
    ## 
    parser.add_argument (
        '-q'                          ,
        '--quiet'                     ,
        action  = "store_true"        ,
        dest    = 'Quiet'             ,
        help    = "``Quiet'' processing"  ,
        default = False   
        )
    ## 
    parser.add_argument (
        '-r'                     ,
        '--root'                 ,
        type    = str            ,
        dest    = 'RootInTES'    ,
        help    = 'Root-In-TES'  ,
        default = ''           
        )
    ##
    parser.add_argument (
        '-u'                       ,
        '--useoracle'              ,
        action  = "store_true"     ,
        dest    = 'UseOracle'      ,
        help    = "Use Oracle-DB"  ,
        default = False   
        )
    ##
    parser.add_argument (
        '-l'                       ,
        '--lumi'                   ,
        action  = "store_true"     ,
        dest    = 'Lumi'           ,
        help    = "Use Lumi?"      ,
        default = False   
        )
    ##
    parser.add_argument (
        '-k'                       ,
        '--klist'                  ,
        dest    = 'FileList'       ,
        type    = str              ,
        default = ''               ,
        help    = "A file with list of input file names"
        )
    ##
    parser.add_argument (
        '-i'                       ,
        '--import'                 ,
        dest    = 'ImportOptions'  ,
        type    = str              ,
        default = ''               ,
        help    = "A file to be used for 'importOptions'"
        )
    ##
    return parser

# =============================================================================
## try to extract the data type, simulation flag and file extension (type)
#
#  @code
#
#    >>> file_names  = ...
#    >>> data_type, simulation,ext = dataType ( file_names )
#
#  @endcode
#
def dataType ( files ) :
    """
    extract the data type,
    simulation type and input type from file name
    
    >>> file_names  = ...
    >>> data_type, simulation,ext = daataType ( file_names )
    
    """
    #
    if isinstance ( files  , str ) : files = [ files ]
    #
    dtype = ''
    simu  = False
    ext   = '' 
    for f in files  :
        #
        if   0 <= f.find ( 'Collision09'   ) : dtype = '2009'
        elif 0 <= f.find ( 'Collision10'   ) : dtype = '2010'
        elif 0 <= f.find ( 'Collision11'   ) : dtype = '2011'
        elif 0 <= f.find ( 'Collision12'   ) : dtype = '2012'
        elif 0 <= f.find ( 'Collision13'   ) : dtype = '2012' ## ATTENTION
        #
        elif 0 <= f.find ( 'Stripping13'   ) : dtype = '2011'
        elif 0 <= f.find ( 'Stripping17'   ) : dtype = '2011'
        elif 0 <= f.find ( 'Stripping15'   ) : dtype = '2011'
        elif 0 <= f.find ( 'Stripping19'   ) : dtype = '2012'
        #
        elif 0 <= f.find ( 'Stripping20r1' ) : dtype = '2011'
        elif 0 <= f.find ( 'Stripping20r0' ) : dtype = '2012'
        #
        elif 0 <= f.find ( '2k+10'         ) : dtype = '2010'
        elif 0 <= f.find ( '2k+11'         ) : dtype = '2011'
        elif 0 <= f.find ( '2k+12'         ) : dtype = '2012'
        elif 0 <= f.find ( '2k+13'         ) : dtype = '2013'
        
        #
        if   0 <= f.find ( 'MC09' ) or 0 <= f.find ( 'MC/2009' ) :
            dtype = '2009'
            simu  = True 
        elif 0 <= f.find ( 'MC10' ) or 0 <= f.find ( 'MC/2010' ) :
            dtype = '2010'
            simu  = True 
        elif 0 <= f.find ( 'MC11' ) or 0 <= f.find ( 'MC/2011' ) :  
            dtype = '2011'
            simu  = True 
        elif 0 <= f.find ( 'MC12' ) or 0 <= f.find ( 'MC/2012' ) :
            dtype = '2012'
            simu  = True
        elif 0 <= f.find ( 'MC13' ) or 0 <= f.find ( 'MC/2013' ) :
            dtype = '2013'
            simu  = True
        elif 0 <= f.find ( '/MC/'   ) : simu = True
        elif 0 <= f.find ( 'Pythia' ) : simu = True
        elif 0 <= f.find ( 'BcVeg'  ) : simu = True
        
        p   = f.rfind ( '.' )
        if 0 <= p :
            ## allow up to 5 symbols for exension 
            if len ( f[p+1:] ) <= 5 : ext = f[p+1:]
            
    return  dtype,simu,ext.upper() 

# =============================================================================
##  get file name from filelist 
def fileList  ( file_list ) :
    """
    Get list of files from filelist
    
    """
    if not file_list : return []
    #
    ## try to read filelist 
    import os
    #
    file_list = os.path.expandvars ( file_list ) 
    file_list = os.path.expanduser ( file_list ) 
    file_list = os.path.expandvars ( file_list ) 
    file_list = os.path.expanduser ( file_list ) 
    file_list = os.path.expandvars ( file_list )
    #
    if not os.path.exists ( file_list ) : return []
    result = []
    #
    with open ( file_list , 'r' ) as mfile :
        for line in mfile :
            if line : result.append ( line )
            
    return result 
    

    
# =============================================================================
## try to extract more information from file names 
def hasInFile ( files , pattern ) :
    """
    extract the data type,
    simulaton type and input type from file name 
    """
    #
    if isinstance ( files  , str ) : files = [ files ]
    #
    ptnu = pattern.upper() 
    for f in files  :
        fu = f.upper() 
        if   0 <= fu.find ( ptnu ) : return True
    return False

# =============================================================================
## try to determine the year 
def theYear ( files , params = {} , the_year = None ) :
    """
    Try to determine the year 
    """
    for key in ( 'year' , 'Year' , 'YEAR' ) :
        if params.has_key ( key ) :
            year = params[key] 
            logger.info ("The Year %s is set from params['%s']" % ( year , key ) )
            return year 

    ## try to it get from file-names 
    year,simu,ext = dataType ( files )
    if year :
        logger.info ("The Year %s is set from filenames" % year )
        return year
    
    year = the_year 
    logger.info ("The Year %s is set from default" % year )
    return year 
    
# =============================================================================
if __name__ == '__main__' :

    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) ) 
    logger.info ( 80*'*'  ) 

# =============================================================================
# The END 
# =============================================================================
