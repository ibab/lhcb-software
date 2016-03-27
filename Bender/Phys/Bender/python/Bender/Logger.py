#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/Logger.py
#
#  Logger for Bender 
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
#  @date   2010-09-12
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""Logger for Bender 

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
with the smear campaign of Dr.O.Callot et al.: 
   ``No Vanya's lines are allowed in LHCb/Gaudi software.''

                    $Revision$
  Last modification $Date$
                 by $Author$  
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
    'colored_string' , ## helper function to colorize the string
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
