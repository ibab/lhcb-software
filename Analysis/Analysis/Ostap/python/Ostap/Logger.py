#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
## @file
#  very simple logger for Ostap 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Very simple logger for Ostap
>>> logger = getLogger('MyLogger')
"""
# =============================================================================
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2012-03-16"
__version__ = '$Revision$'
__all__     = (
    'getLogger'      , ## get (configured) logger
    'setLogging'     , ## set disable level according to MSG.Level
    'LogLevel'       , ## context manager to control output level 
    'logLevel'       , ## helper function to control output level
    'logVerbose'     , ## helper function to control output level
    'logDebug'       , ## helper function to control output level
    'logInfo'        , ## helper function to control output level
    'logWarning'     , ## helper function to control output level
    'logError'       , ## helper function to control output level
    'logColor'       , ## context manager to switch on  color logging locally  
    'logNoColor'     , ## context manager to switch off color logging locally  
    'noColor'        , ## context manager to switch off color logging locally  
    )
# =============================================================================
## import actual logger 
from AnalysisPython.Logger import ( getLogger      , setLogging   , 
                                    logLevel       , logVerbose   , logDebug ,
                                    logInfo        , logWarning   , logError , 
                                    logColor       , logNoColor   , noColor  ,
                                    make_colors    , reset_colors , 
                                    colored_string ) 
# =============================================================================
if '__main__' == __name__ : logger = getLogger ( 'Bender.Logger' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
if __name__ == '__main__' :
    
    import logging
    logging.disable( logging.VERBOSE - 1  ) 
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )
    logger.info ( 80*'*'  ) 

    with logColor() : 
        logger.verbose  ( 80*'*'  )
        logger.debug    ( 80*'*'  )
        logger.info     ( 80*'*'  )
        logger.error    ( 80*'*'  )
        logger.warning  ( 80*'*'  )
        logger.critical ( 80*'*'  )
        logger.fatal    ( 80*'*'  )
        
    logger.verbose  ( 80*'*'  )
    logger.debug    ( 80*'*'  )
    logger.info     ( 80*'*'  )
    logger.error    ( 80*'*'  )
    logger.warning  ( 80*'*'  )
    logger.critical ( 80*'*'  )
    logger.fatal    ( 80*'*'  )

# =============================================================================
# The END 
# =============================================================================

