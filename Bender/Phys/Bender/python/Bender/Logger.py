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
    'getLogger' ,
    'setLogging' 
    )
# =============================================================================
## some manipulations with logging module 
import logging
logging.addLevelName ( logging.CRITICAL  , 'FATAL  '  )
logging.addLevelName ( logging.WARNING   , 'WARNING'  )
logging.addLevelName ( logging.DEBUG     , 'DEBUG  '  )
logging.addLevelName ( logging.INFO      , 'INFO   '  )
logging.addLevelName ( logging.ERROR     , 'ERROR  '  )
if not hasattr ( logging , 'VERBOSE' ) :
    logging.VERBOSE = 5
logging.addLevelName ( logging.VERBOSE   , 'VERBOSE'  )
    
def _verbose1_(self, msg, *args, **kwargs):
        """
        Log 'msg % args' with severity 'VERBOSE'.
        """
        if self.isEnabledFor(logging.VERBOSE):
            self._log(logging.VERBOSE, msg, args, **kwargs)

def _verbose2_(msg, *args, **kwargs):
    """
    Log a message with severity 'DEBUG' on the root logger.
    """
    if len(logging.root.handlers) == 0:
        logging.basicConfig()
    logging.root.verbose (msg, *args, **kwargs)

logging.Logger.verbose = _verbose1_
logging.verbose        = _verbose2_

## convert MSG::Level into logging level 
def setLogging ( output_level ) :
    """Convert MSG::Level into logging level 
    """
    if   6 <= output_level : logging.disable ( logging.FATAL   - 1 )
    elif 5 <= output_level : logging.disable ( logging.ERROR   - 1 )
    elif 4 <= output_level : logging.disable ( logging.WARNING - 1 )
    elif 3 <= output_level : logging.disable ( logging.INFO    - 1 )
    elif 2 <= output_level : logging.disable ( logging.DEBUG   - 1 )
    elif 1 <= output_level : logging.disable ( logging.VERBOSE - 1 )

# =============================================================================
__with_colors_ = False

def with_colors() :
    global __with_colors_
    return __with_colors_ 

def setColors( what ) :
    global __with_colors_
    __with_colors_ = what
    return with_colors()

def with_ipython()  :
    try :
        return __IPYTHON__
    except NameError :
        return False 


## male colors 
def make_colors () :

    ##return

    if with_colors() : return 
    
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
            return '{bg}{name}{reset}' .format (
                bg    = COLOR_SEQ % ( 40 + bg ) ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        elif fg : 
            return '{fg}{bold}{name}{reset}' .format (
                fg    = COLOR_SEQ % ( 30 + fg ) ,
                bold  = BOLD_SEQ ,
                name  = logging.getLevelName ( level ) ,
                reset = RESET_SEQ
                )
        return '{bold}{name}{reset}' .format (
            bold  = BOLD_SEQ ,
            name  = logging.getLevelName ( level ) ,
            reset = RESET_SEQ
            )
    
    import logging

    logging.addLevelName( logging.CRITICAL ,  makeName( logging.CRITICAL , fg = RED    , bg  = BLUE     ) )
    logging.addLevelName( logging.WARNING  ,  makeName( logging.WARNING  , fg = RED    , bg  = YELLOW   ) )
    logging.addLevelName( logging.ERROR    ,  makeName( logging.ERROR    , fg = YELLOW , bg  = RED      ) )
    #logging.addLevelName( logging.INFO     ,  makeName( logging.INFO     , fg = BLUE   )) # , bg  = WHITE    ) )
    #logging.addLevelName( logging.DEBUG    ,  makeName( logging.DEBUG    , fg = GREEN  )) # , bg  = WHITE    ) )
    logging.addLevelName( logging.INFO     ,  makeName( logging.INFO     , bg = BLUE   , fg  = WHITE ) )
    logging.addLevelName( logging.DEBUG    ,  makeName( logging.DEBUG    , bg = GREEN  , fg  = WHITE ) )

    setColors( True ) 
    
    
##  for ipython mode, add colors 
if with_ipython() :
    make_colors()

_columns = 0 

def getLogger ( name ) :
    from AnalysisPython.Logger import getLogger as gL
    import logging
    
    ## global _columns
    ## if  0>= _columns :
    ##     from Ostap.progress_bar import columns
    ##     _columns = columns ()
    ## if   _columns > 160 :
    ##     return gL ( name , fmt = '# %(name)-25s %(levelname)020s %(message)-120s ## %(filename)s/%(lineno)d' )
    ## elif _columns > 120 :
    ##     return gL ( name , fmt = '# %(name)-25s %(levelname)020s %(message)-80s ## %(filename)s'            )
    ## else :

    level = logging.VERBOSE - 1 
    if not with_colors() : return gL ( name , level = level ) 
    return gL ( name , fmt = '# %(name)-25s %(levelname)020s %(message)s' , level = level ) 



# ==============================================================================
## define the default logging threshold  
setLogging ( 3 ) 

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

    make_colors()
    
    if with_colors() :
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
