#!/usr/bin/env python
# =============================================================================
# $Id$
# =============================================================================
# $URL$
# =============================================================================
## @file decorators.py LoKiCore/basic.py
#
#  The set of basic decorator for objects from LoKiCore library
#
#        This file is a part of LoKi project -
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
#  contributions and advices from G.Raven, J.van Tilburg,
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#
#  $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""
The set of basic decorators for objects from LoKiCore library

    This file is a part of LoKi project -
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
contributions and advices from G.Raven, J.van Tilburg,
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__date__    = "2010-09-27"
__version__ = "SVN $Revision$ "
# =============================================================================
__all__     = (
    'cpp'   ,
    'std'   ,
    'LoKi'  ,
    'LHCb'  ,
    'Gaudi'
    )
# =============================================================================

import GaudiPython

# namespaces shortcuts
cpp   = GaudiPython.gbl
std   = GaudiPython.gbl.std
LoKi  = GaudiPython.gbl.LoKi
LHCb  = GaudiPython.gbl.LHCb
Gaudi = GaudiPython.gbl.Gaudi

# =============================================================================
if '__main__' == __name__ :

    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__
    print 80*'*'
    for i in dir() : print i


# =============================================================================
# The END
# =============================================================================
