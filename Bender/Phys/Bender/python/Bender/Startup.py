#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## The trivial startup sctript for python Bender session
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
#  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
#  @date   2006-10-08
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""This is a trivial startup script for python Bender session

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

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
__author__  = 'Vanya BELYAEV Ivan.Belyaev@itep.ru'
__date__    = "2006-10-08"
__version__ = "$Revision$"
__all__     = () 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Bender.Startup' )
else                      : logger = getLogger ( __name__ )
# =============================================================================

try:

    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.bender_history'
    
    def _rename_ ( file , app ) :
        if os.path.exists ( file + app ) :
            if 0 == os.path.getsize( file + app ) : os.remove( file + app )
            else :   _rename_ ( file + app ,        app )
        if os.path.exists ( file       ) :
            if 0 == os.path.getsize( file ) : os.remove( file )
            else : os.rename  ( file       , file + app )

    ## remove/backup the previous history file
    _rename_ ( __history__ , '.OLD' )

    ## write history at the end 
    def _prnt_() :
        logger.info ( 'BENDER history file: %s' % __history__ ) 
        
    ## line completer 
    import rlcompleter
    import readline
    readline.clear_history() 
    readline.parse_and_bind("tab: complete")

    import atexit
    atexit.register ( readline.write_history_file , __history__ )
    atexit.register ( _prnt_ )

    _prnt_() 
    
except:
    pass 


    
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
