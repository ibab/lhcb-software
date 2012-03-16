#!/usr/bin/env python 
# =============================================================================
## @file Benedr/Fixes.py
#   The helper Python module for Bender application 
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
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$ 
# =============================================================================
"""
The helper module for Bender application.
It applies some last-moment (version-dependent) fixes

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
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = '$Revision$'
__all__     = ()
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger(__name__)
# =============================================================================
logger.info ( '*** Fix some "features"' ) 
# =============================================================================
#
# =============================================================================
## "at-exit action 
def _bender_at_exit_ () :
    """
    
    At-Exit action
    
    """
    logger.info ( '*'*120 ) 
    logger.info ( 'custom "atexit" handler is being invoked' ) 
    logger.info ( '*'*120 ) 
    
    from GaudiPython.Bindings import _gaudi 
    if _gaudi :
        logger.info( 'AppMgr.exit() is being invoked' ) 
        _gaudi.exit()

    logger.info ( '*'*120 ) 
    logger.info ( 'custom "atexit" handler has been invoked' ) 
    logger.info ( '*'*120 ) 


import atexit
atexit.register( _bender_at_exit_ )
logger.info( 'add custom "atexit" handler' ) 

# =============================================================================
## Other fixes:
# =============================================================================
import Bender.Fixes_Gaudi
import Bender.Fixes_LoKi

# =============================================================================
if __name__ == '__main__' :
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120

# =============================================================================
# The END 
# =============================================================================
