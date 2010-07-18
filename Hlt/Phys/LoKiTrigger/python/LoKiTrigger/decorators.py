#!/usr/bin/env python
# =============================================================================
## @file 
#  The set of basic decorator for objects from LoKiTrigger library
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @daet 2007-06-09
# =============================================================================
"""
The set of basic decorators for obejcts from LoKiTrigger library

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

By usage of this code one clearly states the disagreement 
with the campain of Dr.O.Callot et al.: 
 ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 
"""
# =============================================================================
__date__     = "2007-05-29"
__version__  = "CVS tag $Name: not supported by cvs2svn $, verison $Revision: 1.6 $ "
__author__   = "Vanya BELYAEV ibelyaev@physics.syr.edu" 
# =============================================================================

from   LoKiTrigger.functions   import *


_name = __name__

# =============================================================================
## make the decoration of all objects fomr this module
def _decorate ( name = _name  ) :
    """
    Make the decoration of all objects from this module
    """
    import LoKiCore.decorators as _LoKiCore
    
    ## 
    return ()                                     ## RETURN

# =============================================================================
## perform the decoration 
_decorated = _decorate ()                         ## ATTENTION 
# =============================================================================

from LoKiTracks.decorators import *

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

