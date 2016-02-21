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
    'getLogger'    ,  ## get logger
    'setLogging'   ,  ## set the global logging level
    'logColor'     ,  ## locally switch on color logging 
    'make_colors'  ,  ## force colored logging 
    'reset_colors' ,  ## reset colored logging 
    )
# =============================================================================
from AnalysisPython.Logger import getLogger 
# =============================================================================
if '__main__' == __name__ : logger = getLogger ( 'Ostap.Logger' )
else                      : logger = getLogger ( __name__       )
# ============================================================================
from AnalysisPython.Logger import *
# ============================================================================
if __name__ == '__main__' :
    
    setLogging  ( 0 )
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )
    logger.info ( 80*'*'  ) 

    
    logger.verbose  ( 'This is DEBUG    message'  ) 
    logger.debug    ( 'This is DEBUG    message'  ) 
    logger.info     ( 'This is INFO     message'  ) 
    logger.warning  ( 'This is WARNING  message'  ) 
    logger.error    ( 'This is ERROR    message'  ) 
    logger.fatal    ( 'This is FATAL    message'  ) 
    logger.critical ( 'This is CRITICAL message'  ) 

    with logColor () : 
        
        logger.verbose  ( 'This is DEBUG    message'  ) 
        logger.debug    ( 'This is DEBUG    message'  ) 
        logger.info     ( 'This is INFO     message'  ) 
        logger.warning  ( 'This is WARNING  message'  ) 
        logger.error    ( 'This is ERROR    message'  ) 
        logger.fatal    ( 'This is FATAL    message'  ) 
        logger.critical ( 'This is CRITICAL message'  ) 

    logger.verbose  ( 'This is DEBUG    message'  ) 
    logger.debug    ( 'This is DEBUG    message'  ) 
    logger.info     ( 'This is INFO     message'  ) 
    logger.warning  ( 'This is WARNING  message'  ) 
    logger.error    ( 'This is ERROR    message'  ) 
    logger.fatal    ( 'This is FATAL    message'  ) 
    logger.critical ( 'This is CRITICAL message'  ) 

# =============================================================================
# The END 
# =============================================================================

