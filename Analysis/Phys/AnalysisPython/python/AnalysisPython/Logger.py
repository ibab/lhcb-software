#!/usr/bin/env python 
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$ 
# =============================================================================
## @file AnalysisPython/Logger.py
#
#  Simple logger for Bender, Ostap, etc.. (copied from Bender)
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
"""Simple logger for Bender, Ostap, etc..  (copied from Bender)

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
    'getLogger'    , ## get (configured) logger
    'setLogging'   , ## set disable level according to MSG.Level
    'LogLevel'     , ## context manager to control output level 
    'logLevel'     , ## helper function to control output level
    'logVerbose'   , ## helper function to control output level
    'logDebug'     , ## helper function to control output level
    'logInfo'      , ## helper function to control output level
    'logWarning'   , ## helper function to control output level
    'logError'     , ## helper function to control output level
    'logColor'     , ## context manager to switch on color logging locally  
    'make_colors'  , ## force colored logging 
    'reset_colors' , ## reset colored logging 
    )
# =============================================================================
import logging
## 1) fix logging names 
logging.addLevelName ( logging.CRITICAL  , 'FATAL  '  )
logging.addLevelName ( logging.WARNING   , 'WARNING'  )
logging.addLevelName ( logging.DEBUG     , 'DEBUG  '  )
logging.addLevelName ( logging.INFO      , 'INFO   '  )
logging.addLevelName ( logging.ERROR     , 'ERROR  '  )
## 2) add level VERBOSE + corresponding methods 
if not hasattr ( logging , 'VERBOSE' ) :
    logging.VERBOSE = 5
logging.addLevelName ( logging.VERBOSE   , 'VERBOSE'  )
# =============================================================================
## Log message with severity 'VERBOSE'
def _verbose1_(self, msg, *args, **kwargs):
    """Log 'msg % args' with severity 'VERBOSE'.
    """
    if self.isEnabledFor(logging.VERBOSE):
            self._log(logging.VERBOSE, msg, args, **kwargs)
# =============================================================================
## Log message with severity 'VERBOSE'
def _verbose2_(msg, *args, **kwargs):
    """Log a message with severity 'VERBOSE' on the root logger.
    """
    if len(logging.root.handlers) == 0:
        logging.basicConfig()
    logging.root.verbose (msg, *args, **kwargs)

# =============================================================================
## add method 'verbose' to logger 
logging.Logger.verbose = _verbose1_
# =============================================================================
## add method 'verbose' to root logger 
logging.verbose        = _verbose2_

# =============================================================================
## convert MSG::Level into logging level and disable corresponding logging
def setLogging ( output_level ) :
    """Convert MSG::Level into logging level
    and disable corresponding logging 
    """
    if   6 <= output_level : logging.disable ( logging.FATAL   - 1 )
    elif 5 <= output_level : logging.disable ( logging.ERROR   - 1 )
    elif 4 <= output_level : logging.disable ( logging.WARNING - 1 )
    elif 3 <= output_level : logging.disable ( logging.INFO    - 1 )
    elif 2 <= output_level : logging.disable ( logging.DEBUG   - 1 )
    elif 1 <= output_level : logging.disable ( logging.VERBOSE - 1 )
    else                   : logging.disable ( logging.NOTSET  + 1 )
    
# =============================================================================
# COLORS: 
# =============================================================================
## global flag to indicate if we use colored logging
__with_colors__ = False
# =============================================================================
def with_colors() :
    global __with_colors__
    return __with_colors__ 
# =============================================================================
## helper function that allows to detect running ipython
def with_ipython()  :
    """Helper function that allows to detect running ipython"""
    try :
        return __IPYTHON__
    except NameError :
        return False

# =============================================================================
## get logger
#  @code
#  logger1 = getLogger ( 'LOGGER1' )
#  logger2 = getLogger ( 'LOGGER2' , level = logging.INFO )
#  @endcode 
def _getLogger_ ( name                                                 ,
                  fmt    = '# %(name)-25s %(levelname)-7s %(message)s' ,
                  level  = logging.VERBOSE - 2                         ,
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
    name                         , 
    fmt    = None                ,
    level  = logging.VERBOSE - 2 ,
    stream = None                ) :  
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
#  with logVerbose() :
#       ...do something... 
#  @endcode
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2014-01-1
def logVerbose () :
    """Temporarily disable all loggers with level less then INFO 
    >>> with logVerbose() :
    >>>  ...do something...
    """    
    return logLevel (  logging.VERBOSE   - 1 )

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
    >>> with logDebug() :
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
def reset_colors() :
    """Reset colorization
    >>> reset_colors()
    """
    logging.addLevelName ( logging.CRITICAL  , 'FATAL  '  )
    logging.addLevelName ( logging.WARNING   , 'WARNING'  )
    logging.addLevelName ( logging.DEBUG     , 'DEBUG  '  )
    logging.addLevelName ( logging.INFO      , 'INFO   '  )
    logging.addLevelName ( logging.ERROR     , 'ERROR  '  )
    logging.addLevelName ( logging.VERBOSE   , 'VERBOSE'  )
    
    global __with_colors__
    __with_colors__ = False 
    
# =============================================================================
## make colors 
def make_colors () :
    """Colorize logging
    """
    if with_colors() : return
    
    # ===================================================================================
    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = range(8)
    #The background is set with 40 plus the number of the color, and the foreground with 30
    #These are the sequences need to get colored ouput
    RESET_SEQ = "\033[0m"
    COLOR_SEQ = "\033[1;%dm"
    BOLD_SEQ  = "\033[1m"
    
    def  makeName ( level , fg = None  , bg = None  ) :
        if bg is None and fg is None : 
            return '{bg}{fg}{name}{reset}' .format (
                fg    = COLOR_SEQ % ( 30 + fg ) ,
                bg    = COLOR_SEQ % ( 40 + bg ) ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        elif bg is None : 
            return '{bg}{bold}{name}{reset}' .format (
                bg    = COLOR_SEQ % ( 40 + bg ) ,
                bold  = BOLD_SEQ  ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        elif fg is None : 
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
## @class ColorLogging
#  Simple context manager to swicth on/off coloring
#  @code
#  with ColorLogging():
#      ... do something ... 
#  @endcode 
class ColorLogging(object) :
    """Simple context manager to swicth on/off coloring
    
    >>> with ColorLogging() :
    ...     do something ... 
    """
    def __enter__ ( self ) :
        make_colors  ()
        return self
    def __exit__  ( self , *_ ) :
        reset_colors ()

# =============================================================================
## simple context manager to swict on/off color logging 
#  @code
#  with logColor() :
#      ... do something ... 
#  @endcode 
def logColor () :
    """Simple context manager to swicth on/off coloring
    
    >>> with logColor () :
    ...     do something ... 
    """
    return ColorLogging ()

# =============================================================================
## reset colors
reset_colors ()
    
# =============================================================================
##  for ipython mode actiavte colors 
if with_ipython() :
    make_colors()

# =============================================================================
## define default logging thresholds as 'INFO'
setLogging ( 3 )

# =============================================================================
if __name__ == '__main__' :


    setLogging ( 0 )
    
    logger = getLogger ( 'AnalysisPython.Logger' ) 
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )
    logger.info ( 80*'*'  )
    
    logger.verbose  ( 'This is VERBOSE  message'  ) 
    logger.debug    ( 'This is DEBUG    message'  ) 
    logger.info     ( 'This is INFO     message'  ) 
    logger.warning  ( 'This is WARNING  message'  ) 
    logger.error    ( 'This is ERROR    message'  ) 
    logger.fatal    ( 'This is FATAL    message'  ) 
    logger.critical ( 'This is CRITICAL message'  ) 

    with logColor() : 
        
        logger.verbose  ( 'This is VERBOSE  message'  ) 
        logger.debug    ( 'This is DEBUG    message'  ) 
        logger.info     ( 'This is INFO     message'  ) 
        logger.warning  ( 'This is WARNING  message'  ) 
        logger.error    ( 'This is ERROR    message'  ) 
        logger.fatal    ( 'This is FATAL    message'  ) 
        logger.critical ( 'This is CRITICAL message'  ) 

    logger.verbose  ( 'This is VERBOSE  message'  ) 
    logger.debug    ( 'This is DEBUG    message'  ) 
    logger.info     ( 'This is INFO     message'  ) 
    logger.warning  ( 'This is WARNING  message'  ) 
    logger.error    ( 'This is ERROR    message'  ) 
    logger.fatal    ( 'This is FATAL    message'  ) 
    logger.critical ( 'This is CRITICAL message'  ) 

# =============================================================================
# The END 
# =============================================================================
