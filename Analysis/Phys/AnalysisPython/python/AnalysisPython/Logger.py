#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file AnalysisPython/Logger.py
#
#  copied from Bender
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
"""Simple logger for Bender, Ostap, etc.. 

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
    'getLogger'   , ## get (configured) logger
    'LogLevel'    , ## context manager to control output level 
    'logLevel'    , ## helper function to control outptu level
    'logDebug'    , ## helper function to control outptu level
    'logInfo'     , ## helper function to control outptu level
    'logWarning'  , ## helper function to control outptu level
    'logError'    , ## helper function to control outptu level
    'make_colors' , ## force colored logging 
    )
# =============================================================================
import logging
# =============================================================================
## helper function that allows to detect running ipython
def with_ipython()  :
    """Helper function that allows to detect running ipython"""
    try :
        return __IPYTHON__
    except NameError :
        return False
 
# =============================================================================
## global flag to indicate if we use colored logging
__with_colors__ = False
# =============================================================================
def with_colors() :
    global __with_colors__
    return __with_colors__ 
# =============================================================================
## switch coloring on for interactive processing 
## if with_ipython() : make_colors()
# =============================================================================
## get logger
#  @code
#  logger1 = getLogger ( 'LOGGER1' )
#  logger2 = getLogger ( 'LOGGER2' , level = logging.INFO )
#  @endcode 
def _getLogger_ ( name                                                 ,
                  fmt    = '# %(name)-25s %(levelname)-7s %(message)s' ,
                  level  = logging.DEBUG - 2                           ,
                  stream = None                                        ) :  
    """Get the proper logger
    >>> logger1 = getLogger ( 'LOGGER1' )
    >>> logger2 = getLogger ( 'LOGGER2' , level = logging.INFO )
    """
    #
    logger = logging.getLogger ( name )
    logger.propagate =  False 
    ##logger.propagate =  True
    #
    while logger.handlers :
        logger.removeHandler ( logger.handlers[0] )
    #
    if not stream :
        import sys
        stream = sys.stdout
        
    lh  = logging.StreamHandler ( stream ) 
    fmt = logging.Formatter     ( fmt    )
    lh  . setFormatter          ( fmt    )
    logger.addHandler           ( lh     ) 
    #
    logger.setLevel             ( level  )
    #
    return logger

# =============================================================================
## Get the configured  logger with given name
#  @code 
#  logger = getLogger('MyLogger')
#  logger.info  ( 'this is info' ) 
#  logger.debug ( 'this is debug')
#  @endcode 
def getLogger (
    name                       , 
    fmt    = None              ,
    level  = logging.DEBUG - 2 ,
    stream = None              ) :  
    """Get the configured  logger with given name
    >>> logger = getLogger('MyLogger')
    >>> logger.info  ( 'this is info' ) 
    >>> logger.debug ( 'this is debug') 
    """
    #
    if   not fmt and with_colors() :
        fmt = '# %(name)-25s %(levelname)020s %(message)s'
    elif not fmt :
        fmt = '# %(name)-25s %(levelname)-7s %(message)s' 

    return _getLogger_ ( name , fmt , level , stream ) 

# =============================================================================
## @class LogLevel
#  Temporarily enable/disable certain logger levels
#  @code
#  with LogLevel( logging.CRITICAL ) :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
class LogLevel(object) :
    """Temporarily enable/disable certain logger levels
    >>> with LogLevel( logging.CRITICAL ) :
    ...  do something here ...
    """
    def __init__  ( self , level = logging.INFO - 1 ) :
        self.new_level = level 
        self.old_level = logging.root.manager.disable

    ## context manager: ENTER 
    def __enter__ ( self ) :
        self.old_level = logging.root.manager.disable
        logging.disable ( self.new_level )
        return self

    ## context manager: EXIT 
    def __exit__ ( self , *_ ) :        
        logging.disable ( self.old_level )


# =============================================================================
#  Temporarily enable/disable certain logger levels
#  @code
#  with logLevel( logging.CRITICAL ) :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logLevel ( level = logging.INFO - 1 ) :
    """Temporarily enable/disable certain logger levels
    >>> with logLevel( logging.CRITICAL ) :
    >>>  ...do something...
    """
    return LogLevel ( level )

# =============================================================================
#  Temporarily enable/disable all loggers with level less then DEBUG 
#  @code
#  with logInfo() :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logDebug   () :
    """Temporarily disable all loggers with level less then INFO 
    >>> with logInfo() :
    >>>  ...do something...
    """    
    return logLevel (  logging.DEBUG   - 1 )
# =============================================================================
#  Temporarily enable/disable all loggers with level less then INFO 
#  @code
#  with logInfo() :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logInfo    () :
    """Temporarily disable all loggers with level less then INFO 
    >>> with logInfo() :
    >>>  ...do something...
    """
    return logLevel (  logging.INFO    - 1 )

# =============================================================================
#  Temporarily enable/disable all loggers with level less then WARNING
#  @code
#  with logInfo() :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logWarning () : 
    """Temporarily disable all loggers with level less then WARNING
    >>> with logWarning() :
    >>>  ...do something...
    """   
    return logLevel (  logging.WARNING - 1 )

# =============================================================================
#  Temporarily enable/disable all loggers with level less then ERROR 
#  @code
#  with logError() :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logError   () :
    """Temporarily disable all loggers with level less then ERROR
    >>> with logWarning() :
    >>>  ...do something...
    """       
    return logLevel (  logging.ERROR   - 1 )

# =============================================================================
## reset colorization    
logging.addLevelName ( logging.CRITICAL  , 'FATAL  '  )
logging.addLevelName ( logging.WARNING   , 'WARNING'  )
logging.addLevelName ( logging.DEBUG     , 'DEBUG  '  )
logging.addLevelName ( logging.INFO      , 'INFO   '  )
logging.addLevelName ( logging.ERROR     , 'ERROR  '  )
# =============================================================================
## make colors 
def make_colors () :
    """Colorize logging
    """ 
    # ===================================================================================
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)
    #The background is set with 40 plus the number of the color, and the foreground with 30
    #These are the sequences need to get colored ouput
    RESET_SEQ = "\033[0m"
    COLOR_SEQ = "\033[1;%dm"
    BOLD_SEQ  = "\033[1m"
    
    def  makeName ( level , fg = None  , bg = None  ) :
        if bg and fg : 
            return '{bg}{fg}{name}{reset}' .format (
                fg    = COLOR_SEQ % ( 30 + fg ) ,
                bg    = COLOR_SEQ % ( 40 + bg ) ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        elif bg : 
            return '{bg}{bold}{name}{reset}' .format (
                bg    = COLOR_SEQ % ( 40 + bg ) ,
                bold  = BOLD_SEQ  ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        elif fg : 
            return '{fg}{bold}{name}{reset}' .format (
                fg    = COLOR_SEQ % ( 30 + fg ) ,
                bold  = BOLD_SEQ  ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        return '{bold}{name}{reset}' .format (
            bold  = BOLD_SEQ ,
            name  = logging.getLevelName ( level ) ,
            reset = RESET_SEQ
            )
    
    logging.addLevelName( logging.CRITICAL ,  makeName( logging.CRITICAL , fg = RED    , bg  = BLUE     ) )
    logging.addLevelName( logging.WARNING  ,  makeName( logging.WARNING  , fg = RED    , bg  = YELLOW   ) )
    logging.addLevelName( logging.ERROR    ,  makeName( logging.ERROR    , fg = YELLOW , bg  = RED      ) )
    logging.addLevelName( logging.INFO     ,  makeName( logging.INFO     , bg = BLUE   , fg  = WHITE ) )
    logging.addLevelName( logging.DEBUG    ,  makeName( logging.DEBUG    , bg = GREEN  , fg  = WHITE ) )
    
    global __with_colors__
    __with_colors__ = True 

# =============================================================================
if __name__ == '__main__' :
    
    logger = getLogger ( 'AnalysisPython.Logger' ) 
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )
    logger.info ( 80*'*'  )
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
