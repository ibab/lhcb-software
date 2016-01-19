#!/usr/bin/env python
# =============================================================================
# $Id: Startup.py 173921 2014-06-16 14:25:50Z ibelyaev $ 
# =============================================================================
## The trivial startup sctript for Ostap session
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
#                    $Revision: 173921 $
#  Last modification $Date: 2014-06-16 16:25:50 +0200 (Mon, 16 Jun 2014) $
#                 by $Author: ibelyaev $ 
# =============================================================================
"""This is a trivial startup script for Ostap session
 
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
__version__ = "$Revision: 173921 $"
__all__     = () 
# =============================================================================
## logging
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' == __name__ : logger = getLogger ( 'Ostap.Startup' )
else                      : logger = getLogger ( __name__ )
# =============================================================================

try:

    import os
    ## define the name of the history file
    __history__ = os.path.curdir + os.sep + '.ostap_history'
    
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
        logger.info ( 'OSTAP history file: %s' % __history__ ) 
        
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
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
