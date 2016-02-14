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
    )
# =============================================================================
import logging



def with_ipython()  :
    try :
        return __IPYTHON__
    except NameError :
        return False 

_with_colors_ = False

def make_colors () :

    ##return

    global _with_colors_
    if _with_colors_ : return
    
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

    logging.addLevelName( logging.CRITICAL  , '  FATAL'  )
    logging.addLevelName( logging.WARNING   , 'WARNING'  )
    logging.addLevelName( logging.DEBUG     , '  DEBUG'  )
    logging.addLevelName( logging.INFO      , '   INFO'  )
    logging.addLevelName( logging.ERROR     , '  ERROR'  )
    
    
    logging.addLevelName( logging.CRITICAL ,  makeName( logging.CRITICAL , fg = RED    , bg  = BLUE     ) )
    logging.addLevelName( logging.WARNING  ,  makeName( logging.WARNING  , fg = RED    , bg  = YELLOW   ) )
    logging.addLevelName( logging.ERROR    ,  makeName( logging.ERROR    , fg = YELLOW , bg  = RED      ) )
    logging.addLevelName( logging.INFO     ,  makeName( logging.INFO     , fg = BLUE   )) # , bg  = WHITE    ) )
    logging.addLevelName( logging.DEBUG    ,  makeName( logging.DEBUG    , fg = GREEN  )) # , bg  = WHITE    ) )

    global _with_colors_
    _with_colors_ = True
    
    
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

    global _with_colors_
    if not _with_colors_ : return gL ( name ) 
    return gL ( name , fmt = '# %(name)-25s %(levelname)020s %(message)s' )

if '__main__' == __name__ : logger = getLogger ( 'Bender.Logger' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
if __name__ == '__main__' :
    
    import logging
    logging.disable( logging.DEBUG - 5  ) 
    
    logger.info ( 80*'*'  ) 
    logger.info ( __doc__ ) 
    logger.info ( ' Author  : %s ' %  __author__  ) 
    logger.info ( ' Version : %s ' %  __version__ ) 
    logger.info ( ' Date    : %s ' %  __date__    ) 
    logger.info ( ' Symbols : %s ' %  list ( __all__ ) )

    global _with_colors_ 
    if _with_colors_ :
        logger.debug    ( 80*'*'  )
        logger.info     ( 80*'*'  )
        logger.error    ( 80*'*'  )
        logger.warning  ( 80*'*'  )
        logger.critical ( 80*'*'  )
        logger.fatal    ( 80*'*'  )
        logger.debug    ( 80*'*'  )
        logger.info     ( 80*'*'  )
        logger.error    ( 80*'*'  )
        logger.warning  ( 80*'*'  )
        logger.critical ( 80*'*'  )
        logger.fatal    ( 80*'*'  )
        
# =============================================================================
# The END 
# =============================================================================
