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
    'getLogger'   ,  ## get logger
    'make_colors' ,  ## force colored logging 
    )
# =============================================================================
from AnalysisPython.Logger import getLogger, make_colors
# =============================================================================
if '__main__' == __name__ : logger = getLogger ( 'Ostap.Logger' )
else                      : logger = getLogger ( __name__       )
# ============================================================================

# ============================================================================
if __name__ == '__main__' :
    
    import logging
    logging.disable( logging.DEBUG - 5  ) 
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )
    logger.info ( 80*'*'  ) 
    
    ## make colors:
    from Ostap.Logger import with_ipython
    if with_ipython() : make_colors()
    
    logger.debug    ( 'This is DEBUG    message'  ) 
    logger.info     ( 'This is INFO     message'  ) 
    logger.warning  ( 'This is WARNING  message'  ) 
    logger.error    ( 'This is ERROR    message'  ) 
    logger.fatal    ( 'This is FATAL    message'  ) 
    logger.critical ( 'This is CRITICAL message'  ) 

# =============================================================================
# The END 
# =============================================================================

