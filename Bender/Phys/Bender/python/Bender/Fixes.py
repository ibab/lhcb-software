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
"""Apply some last-moment (version-dependent) fixes

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
if '__main__' == __name__ : logger = getLogger ( 'Bender.Fixes' )
else                      : logger = getLogger ( __name__ )
# =============================================================================
#
# =============================================================================
## "at-exit action 
def _bender_at_exit_ () :
    """
    At-Exit action
    """

    logger.debug ( '*'*120 ) 
    logger.debug ( 'custom "atexit" handler is being invoked' ) 
    logger.debug ( '*'*120 ) 
    
    from GaudiPython.Bindings import _gaudi
    rc = None 
    if _gaudi :
        logger.debug ( 'AppMgr.exit() is being invoked' )
        rc = _gaudi.ReturnCode 
        _gaudi.exit  ()

    logger.debug ( '*'*120 ) 
    logger.debug ( 'custom "atexit" handler has been invoked' ) 
    logger.debug ( '*'*120 )

    if rc :

        """     
        const int Success                  = 0x00;
        const int GenericFailure           = 0x01;
        /// @defgroup loop_stop Loop termination
        /// Error codes for abnormal loop termination.
        /// @{
        const int FailInput                = 0x02; //< Error opening file
        const int AlgorithmFailure         = 0x03; //<
        const int ScheduledStop            = 0x04; //< Loop terminated because of user request
        const int IncidentFailure          = 0x05; //< Fatal error in Incident handling
        const int UnhandledException       = 0x06; //<
        const int CorruptedInput           = 0x10; //< Input file showed a corruption
        /// @}
        /// @{
        /// Error codes for operation failures.
        const int FinalizationFailure      = 0x0b;
        /// @}
        const int SignalOffset             = 0x80; //< Offset for signal-related return codes
        """
        
        nc = '0x%x' % rc 
        if   0x00 == rc : nc = 'Success'
        elif 0x01 == rc : nc = 'GenericFailure'
        elif 0x02 == rc : nc = 'FailInput'
        elif 0x03 == rc : nc = 'AlgorithmFailure'
        elif 0x04 == rc : nc = 'ScheduledStop'
        elif 0x05 == rc : nc = 'IncidentFailure'
        elif 0x06 == rc : nc = 'UnhandledException'
        elif 0x10 == rc : nc = 'CorruptedInput'
        elif 0x0b == rc : nc = 'FinalizationFailure'
        
        logger.info ( 'Call sys.exit ("%s")' % nc )
        import sys 
        sys.exit ( rc ) 

import atexit
atexit.register( _bender_at_exit_ )
logger.debug ( 'add custom "atexit" handler' ) 

# =============================================================================
## Other fixes:
# =============================================================================
import Bender.Fixes_Gaudi
import Bender.Fixes_LoKi

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
