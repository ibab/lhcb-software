#!/usr/bin/env python
# =============================================================================
# @file grid_url.py
#
# Helper module to get Grid access URL 
# Actually it is a bit modified version of dirac-dms-lfn-accessURL
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>``Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis''</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  ``C++ ToolKit for Smart and Friendly Physics Analysis''
#
#  By usage of this code one clearly states the disagreement 
#  with the smear campaign of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author VanyaBELYAEV Ivan.Belyaev@itep.ru
# @date   2012-10-23
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
""" Helper module to get Grid access URL for LFN
Actually it is a wrapper for dirac-dms-lfn-accessURL script 

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-10-27"
__version__ = "$Revision$"
__all__     = ( 'accessURLs' , ) 
# =============================================================================
import ROOT,sys  
ROOT.PyConfig.IgnoreCommandLineOptions = True
# =============================================================================
from Bender.Logger import getLogger
if '__main__' == __name__ : logger = getLogger ( 'Bender/grid_url' )
else                      : logger = getLogger ( __name__          )
# =============================================================================
## get the dictionary { 'LFN' : 'access-URL' }
#  Actually it is a wrapper for dirac-dms-lfn-accessURL script 
#  @author VanyaBELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23
def _accessURLs_ ( lfn , sites  = [] , elements = [] ) :  
    """ Get the dictionary { 'LFN' : 'access-URL' }
    Actually it is a wrapper for dirac-dms-lfn-accessURL script 
    """
    
    arguments = [ 'dirac-dms-lfn-accessURL' ]
    arguments.append ( lfn )
    if sites    : arguments += [ '-g' ] + sites
    if elements : arguments += [ '-S' ] + elements 

    import os 
    from subprocess import Popen, PIPE
    logger.debug ( 'use Popen(%s)' % arguments )  
    pipe = Popen ( arguments     ,
                   stdout = PIPE ,
                   stderr = PIPE )
    
    with_errors = False 
    for e in pipe.stderr :
        logger.error( 'STDERR: %s' % arguments[0] )  
        with_errors = True
        
    if with_errors : return {}
    
    Successful = False

    result     = {}
    current    = None 
    for l in pipe.stdout :
        line = l[:-1]
        logger.debug('STDOUT: %s' % line) 
        if not Successful or 0 == line.find('Successful') :
            Successful = True
            continue
        ## parse only lines after the first successful 
        if not Successful : continue
        left,sep,right = line.partition(':') ## primitive "parsing"
        if not sep :
            logger.warning('Invalid line: "%s"' % line )
            continue
        left   = left .strip()
        right  = right.strip()
        if   left        and not right             : current = left 
        elif left == lfn and     right and current : result [ current ] = right 
        else :  logger.warning('Invalid tokens: %s' % tokens )
        

    
    if result :
        logger.info ( 80*'*')
        logger.info ( "LFN : %s" %  lfn )
        keys = result.keys()
        keys.sort() 
        logger.info                 ( "%20s : %s" % ( "Site" , "URL" ) ) 
        for k in keys : logger.info ( "%20s : %s" % ( k , result[k]  ) )
        logger.info ( 80*'*')
    else :
        logger.warning  ( "No valid replicas for LFN %s is found " %  lfn )
        
    return result


# =============================================================================
## get the dictionary { 'LFN' : 'access-URL' }
#  Actually it is wrapper over dirac-dms-lfn-accessURL script
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-23
def accessURLs ( args = [] ) :
    """Get (optionally silently) the map of access URSL for LFNs
    [Actually it is wrapper over dirac-dms-lfn-accessURL script]
    >>> urls = accessURLs() 
    """
    # =========================================================================
    ## make parser: 
    # =========================================================================
    import argparse 
    parser = argparse.ArgumentParser(
        prog        = 'get-grid-ulg/grid_url' ,
        description = """
        Helper module to get Grid access-URL for given LFN.
        [Actually it is wrapper over dirac-dms-lfn-accessURL script]
        """
        )
    
    parser.add_argument (
        "lfn"    ,
        metavar  = "LFN"   ,
        type     = str     , 
        help     = "LFN to be investigated" )
    
    parser.add_argument (
        "-g"       ,
        "--Sites"  ,
        metavar  = "SITES"   ,
        dest     = 'Sites'   ,
        nargs    = '*'       , 
        default  = []        , 
        help     = "GRID sites to consider [defaut : %(default)s ]"
        )
    
    parser.add_argument (
        "-S"     ,
        "--SEs"  ,
        metavar  = "SEs"   ,
        dest     = 'SEs'   ,
        default  = []      , 
        nargs    = '*'     , 
        help     = "Storage Elements to consider [defaut : %(default)s ]"
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
    
    ## use parser:
    import sys    
    if not args : args = [ a for a in sys.argv[1:] if '--' != a ]
    
    config  = parser.parse_args ( args ) 
    silent  = not config.Verbose
    result  = {}
    
    try :
        
        from Ostap.Utils import mute
        with mute ( silent , silent ) :
            result = _accessURLs_ ( config.lfn   ,
                                    config.Sites ,
                                    config.SEs   )                                               
    except:
        if not silent :
            logger.error('Exception caught:' , exc_info = True )  
            result = {}
            
    return result 

# =============================================================================
if __name__ == '__main__' :
    
    print accessURLs () 
        
# =============================================================================
# The END 
# =============================================================================
