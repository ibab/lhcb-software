#!/usr/bin/env python 
# =============================================================================
# $Id$ 
# =============================================================================
## @file Bender/Main.py
#  The major Python module for Bender application 
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
This is a major Python Module for Bender application

This file is a part of BENDER project:
    ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the LoKi project:
    ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campain of Dr.O.Callot et al.: 
    ``No Vanya's lines are allowed in LHCb/Gaudi software.''

  Last modification $Date$
                 by $Author$ 
"""
# =============================================================================
__author__  = 'Vanya BELYAEV ibelyaev@physics.syr.edu'
__date__    = "2004-07-11"
__version__ = '$Revision$' 
# =============================================================================
## logging
# =============================================================================
from Bender.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================

import os 

try:
    ## try to get the startup script from environment 
    startup = os.environ.get('PYTHONSTARTUP',None)
    ## use the default startup script
    if not startup :
        startup = os.sep + 'Bender' + os.sep + 'Startup.py'
        bp = os.environ.get('BENDERPYTHON',None)
        if bp : startup = bp + startup
        else  : startup = 'Startup.py'
    
    if os.path.exists( startup ) :
        execfile( startup )
    else :
        import Bender.Startup
        
    logger.info ( 'Define the startup file to be %s' % startup )

except:
    pass

from PyCintex import gbl as cpp 
            
# =============================================================================

## massive imports of everything-II
logger.info ( "Massive   import of ``everything''")
from LoKiCore.decorators           import *
from LoKiPhys.decorators           import *
from LoKiArrayFunctors.decorators  import *
from LoKiAlgo.decorators           import *
from LoKiCore.functions            import *


## massive imports of everything-II
logger.info ( "Even more imports of ``everything''")
## from LoKiTrigger.decorators        import *
from LoKiTracks.decorators         import *
from LoKiProtoParticles.decorators import *
from LoKiHlt.decorators            import *
from LoKiNumbers.decorators        import *

# =============================================================================
from Gaudi.Configuration import importOptions

from Bender.Utils import * 

# =============================================================================
## apply some last-minutes fixes
import Bender.Fixes

## @var LoKi   : define namespace LoKi 
LoKi   = cpp.LoKi
## @var Bender : define namespace Bender 
Bender = cpp.Bender
## @var LHCb   : define namespace LHCb
LHCb   = cpp.LHCb
## @var Gaudi  : define namespace Gaudi
Gaudi  = cpp.Gaudi

_SC     = cpp.StatusCode
SUCCESS = _SC(_SC.SUCCESS,True)
FAILURE = _SC(_SC.FAILURE,True)

from GaudiKernel.PhysicalConstants import c_light

# =============================================================================
## Welcome message:
Bender.Welcome.instance()

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
